// Copyright 2023 Robert M. Wolff (bob dot wolff 68 at gmail dot com)
//
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this 
// list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, 
// this list of conditions and the following disclaimer in the documentation and/or 
// other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors 
// may be used to endorse or promote products derived from this software without 
// specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
#ifndef ROBOTASK_H_
#define ROBOTASK_H_
#ifdef ESP_PLATFORM
#include <Arduino.h>
#else
#include <thread>
#include <chrono>
#include <mutex>
#endif

#ifdef __AVR__
#include <Arduino_FreeRTOS.h>
#define ROBOSTACKSIZE 128
#else
// ESP32 case
#define ROBOSTACKSIZE 2048
#endif

#include <cstdint>

#define MAXTASKNAMELEN 32

/**
 * @author Robert Wolff - based largely on Tom Bottglieri's work from FRC Team 254
 * @author Tom Bottglieri
 *
 * @brief Abstract superclass of tasks (run on a separate thread).
 * Call Start() to begin the task and Pause() to temporarily pause it.
 * The inheriting class must implement Run() which will be called again and
 * again when the task is in 'Start/Running' mode.
 */

class RoboTask {
 public:
  /**
   * @brief Constructor which takes an optional taskname. In the absence of a task name,
   *        a task name will be created based upon the current system-time.
   */
  RoboTask(const char* _taskName = nullptr, uint8_t priority=1, int stacksize=ROBOSTACKSIZE);
  virtual ~RoboTask();

  /**
   * @brief PRIVATE- Do not call this function externally as it's only passed to the constructor to initialize the object.
   *
   * This function sets up an operating-system task which will perpetually loop the Run() function.
   * @note When running, loops wait minimally between calls to Run(). During pause, loop waits longer or actually suspends the task depending on implementation.
   * @param task is a FreeRTOS task to run.
   */
  static void RoboPrivateStarterTask(void* task);

  /**
   * @brief Starts the task.
   */
  void Start();

  /**
   * @brief Pauses the task.
   */
  void Pause();

  /**
   * @brief Terminate the task.
   */
  void Terminate();

  /**
   * @brief Check to see if the task is still alive.
   */
  bool isDead();

  /**
   * @brief Check to see if the task is Alive & Running.
   */
  bool isPaused();

  /**
   * @brief Easy way to utilize the base class to set a lower 'action time' frequency
   */
  bool hasElapsed(unsigned long elapsed);

  /**
   * @brief Start the elapsed timer again with the current time.
   */
  void resetElapsedTimer();

  /**
   * @brief Change the default delay period in the task manager base class.
   */
  void setBaseRunDelay(uint32_t delay);

  /**
   * @brief Sleep/yield for a period of milliseconds. Handles native C++ method and FreeRTOS method.
  */
  virtual void sleepMS(u_int32_t ms);
  
  /**
   * @brief The inheriting class must implement this function. This is the function
   *        which gets run in the new task.
   *
   * Subclasses should override this function to specify their own behavior.
   */
  virtual void Run() = 0;

  bool isThisThreadContext();

  uint16_t getStackHighWaterMark();

 private:
  bool enabled_;
  bool bConfirmedPaused;
  bool running_;
  bool isDead_;
  unsigned long runDelayPeriod;
  char taskName[MAXTASKNAMELEN+1];
protected:
  uint8_t slowRunChecks;  // Used to track contiguous runs that are slower than expected.
  bool useLocking;
  static bool isLocked;
#ifdef ESP_PLATFORM
  TaskHandle_t Task_Handler;
  unsigned long startTimer;
  // All RoboTask entities share this locking mutex
  static SemaphoreHandle_t xMutex;
#else
  std::thread* pThread;
  std::chrono::high_resolution_clock::time_point startTimePoint;
  std::thread::id this_thread_id;
  static std::mutex* xMutex;
#endif
};

/**
 * @author Robert Wolff
 *
 * @brief Version of RoboTask which utilizes a Mutex lock around the call to Run(). 
 * @details The mutex here is static and shared amongst all LockingRoboTask objects. And note that
 *          the source shows that the regular RoboTask holds the mutex but this is for the 
 *          convenience of having a single PrivateStarterTask which handles both situations of 
 *          with and without locks. Also, after a single instance of LockingRoboTask is 
 *          instantiated, others who are non-task-based can utilize taking and giving the mutex
 *          by the static TakeMutex() and GiveMutex() methods.
 *       
 */
class LockingRoboTask : public RoboTask {
public:
  LockingRoboTask(const char* taskName = nullptr, uint8_t priority=1, int stacksize=ROBOSTACKSIZE);
  /**
   * @brief Allows user to toggle the usage of the mutex - unusual case but could be helpful.
  */
  void enableLocking();
  /**
   * @brief Allows user to toggle the usage of the mutex - unusual case but could be helpful.
  */
  void disableLocking();
  /**
   * @brief Allows an external non-robotask oriented function to participate in the use of the
   *        mutex mechanism. Use this take before your shared/critical area.
  */
  static void TakeMutex();
  /**
   * @brief Allows an external non-robotask oriented function to participate in the use of the
   *        mutex mechanism. Use this give after your shared/critical area.
  */
  static void GiveMutex();

  /**
   * @brief Sleep/yield for a period of milliseconds. Handles native C++ method and FreeRTOS method.
   * 
   * @details Note the LockingRoboTask would be in the middle of a lock if the caller were calling
   *          from inside their Run() implementation. It is preferable for the Run() to utilize the
   *          built-in setBaseRunDelay() function, but this version of sleepMS() will properly give
   *          back the mutex and re-take it after the sleep.
   *          WARNING: Any Run() implementation for LockedRoboTask which calls a system version of
   *                   sleep like vTaskDelay() will wreak havoc on the system as the mutexes will
   *                   be artifically locked for that duration.
  */
  void sleepMS(u_int32_t ms);
};

#endif  // ROBOTASK_H_
