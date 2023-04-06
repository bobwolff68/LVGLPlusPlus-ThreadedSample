/*******************************
 * 
 * FILE ONLY GETS COMPILED IN ESP32 BUILD MODE
 * 
 * ****************************/

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

//
// Many implementation 'cues' were taken from the LVGL platformio sample project for 'startup' actions
//      https://github.com/lvgl/lv_platformio/tree/master/hal/sdl2
//

#include <Arduino.h>
#include "TFT_eSPI.h"
#include "Widgets.h"

extern void instantiateCommonItems();

TFT_eSPI tft = TFT_eSPI();

LV_IMG_DECLARE(ON1);

#if LV_USE_LOG
/* How important log should be added:
 * LV_LOG_LEVEL_TRACE       A lot of logs to give detailed information
 * LV_LOG_LEVEL_INFO        Log important events
 * LV_LOG_LEVEL_WARN        Log if something unwanted happened but didn't cause a problem
 * LV_LOG_LEVEL_ERROR       Only critical issue, when the system may fail
 * LV_LOG_LEVEL_NONE        Do not log anything
 */
#  define LV_LOG_LEVEL    LV_LOG_LEVEL_WARN
#endif

void RecalibrateTouchScreen() {
    int cornerSize = 20;
    int yStart=cornerSize+25;
    int ySpacing;
    uint16_t calData[5];

    tft.setTextColor(0x3A2D, 0xE6D6);
    tft.fillScreen(0xE6D6);
//    pTFT->setFreeFont(FSS18);
    tft.setTextDatum(TC_DATUM);
    ySpacing = tft.fontHeight();

    tft.drawString("Must Calibrate", SDL_HOR_RES/2, yStart + 0*ySpacing);
    tft.drawString("Touchscreen", SDL_HOR_RES/2, yStart + 1*ySpacing);

    yStart = yStart + 2*ySpacing;
//    pTFT->setFreeFont(FSS12);
    ySpacing = tft.fontHeight();
    tft.drawString("Touch corners", SDL_HOR_RES/2, yStart + 0*ySpacing);
    tft.drawString("where indicated.", SDL_HOR_RES/2, yStart + 1*ySpacing);

    tft.calibrateTouch(calData, 0x3E08, 0x0000, cornerSize);

    tft.fillScreen(0xE6D6);

    tft.setTouch(calData);

    Serial.printf("RecalibrateTouchScreen - Raw calData[] = { %d, %d, %d, %d, %d\n", 
                calData[0], calData[1], calData[2], calData[3], calData[4]);
}

#if 1
void my_print(const char * buf)
{
    Serial.printf(buf);
    Serial.flush();
}

void displayFlush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
    tft.endWrite();

    lv_disp_flush_ready( disp );
}

void touchscreen_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data ) {
  uint16_t touchX, touchY;
  bool touched = tft.getTouch( &touchX, &touchY, 600);
  if( !touched )
  {
      data->state = LV_INDEV_STATE_REL;
  }
  else
  {
      data->state = LV_INDEV_STATE_PR;
      /*Set the coordinates*/
      data->point.x = touchX;
      data->point.y = touchY;
      /*
      Serial.print( "Data x " );
      Serial.println( touchX );

      Serial.print( "Data y " );
      Serial.println( touchY );
      */
  }
}
#endif

void lvSetup(){
  /*

  init

  */
  lv_init();

  #if LV_USE_LOG != 0
    lv_log_register_print_cb( my_print ); /* register print function for debugging */
  #endif

  /*

  setup del buffer

  */
  static lv_disp_draw_buf_t display_buffer;
  //RMW - Dividing by 10 here is all about how LVGL uses a smaller buffer to do data transfers
  //      for the display. The value '10' is not a magic number. Smaller or bigger would work
  //      with consequences to memory and/or performance depending on which way you go.
  #define BUFFER_DIVIDER_FACTOR 10
  static lv_color_t image_buffer[SDL_HOR_RES  * SDL_VER_RES / BUFFER_DIVIDER_FACTOR];
  lv_disp_draw_buf_init(&display_buffer, image_buffer, NULL, SDL_HOR_RES  * SDL_VER_RES / BUFFER_DIVIDER_FACTOR);

  /*

  setup del dislpay

  */
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);  
  disp_drv.hor_res = SDL_HOR_RES;
  disp_drv.ver_res = SDL_VER_RES;
  disp_drv.flush_cb = displayFlush;
  disp_drv.draw_buf = &display_buffer;
  lv_disp_t * disp = lv_disp_drv_register(&disp_drv);

  /*

  setup dell'input (touchscreen)

  */
  static lv_indev_drv_t indev_drv;           /*Descriptor of a input device driver*/
  lv_indev_drv_init(&indev_drv);             /*Basic initialization*/
  indev_drv.type = LV_INDEV_TYPE_POINTER;    /*Touch pad is a pointer-like device*/
  indev_drv.read_cb = touchscreen_read;      /*Set your driver function*/
  lv_indev_drv_register(&indev_drv);         /*Finally register the driver*/

  /*

  Setup del tema

  */
  //lv_theme_t * th = lv_theme_basic_init(disp);
  lv_theme_t * th = lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);
  lv_disp_set_theme(disp, th);

}

//
// GPIO2 is the blue LED pin. It can be useful to tell if the chip/program "booted" by blinking
//   this LED at startup.
// However, it is also the CHIP_SELECT for the TFT display.
// So, this needs to be done prior to tft.init() is called.
//
#define PIN_TFT_CS  2

void blinkLED_GPIO2(uint8_t qty, uint16_t ms) {
  pinMode(PIN_TFT_CS, OUTPUT);

  for (uint8_t i=0; i<qty; i++) {
    digitalWrite(PIN_TFT_CS, HIGH);
    delay(ms);
    digitalWrite(PIN_TFT_CS, LOW);
    delay(ms);
  }
}

class LVTaskHandler : public LockingRoboTask {
public:
  LVTaskHandler() : LockingRoboTask("LVGLLckRoboTsk", 1, 8192) {
    setBaseRunDelay(15);
    Start();
  };
  void Run() {
    lv_task_handler();
  };
};

static LVTaskHandler *plvTask=nullptr;

void mySetup() {
  blinkLED_GPIO2(1, 500);

  Serial.begin(115200);
  while (!Serial)
    ;

  blinkLED_GPIO2(3, 300);

  tft.begin();
  tft.setRotation(MMB_ROTATION);
//  ledcAttachPin(TFT_BL, 1);      // assign TFT_BL pin to channel 1
//  ledcSetup(1, 12000, 8);        // 12 kHz PWM, 8-bit resolution
//  ledcWrite(1, TFT_BRIGHTNESS);  // brightness 0 - 255

  tft.fillScreen(0x0C84); // Dk-green from roboswamp
//  tft.setSwapBytes(true);
//  tft.drawLine(0, 0, 200, 200, 0xFFE0);

#if 0
RecalibrateTouchScreen();
#else
  // ILI9341
  uint16_t calData[] = { 406, 3410, 278, 3463, 7 };
//  uint16_t calData[5] = { 484, 3387, 329, 3256, 1 };
  //uint16_t calData[5] = { 403, 3442, 419, 3251, 1 };
  // verticale
  //uint16_t calData[5] = { 295, 3493, 320, 3602, 2 };
  // orizzontale invertito
  //uint16_t calData[5] = {418, 3404, 334, 3324, 7};  

  tft.setTouch(calData);
#endif
  /*

  lvgl setup
 
  */
  lvSetup();
  
}

void setup() {
  mySetup();

LV_LOG("Starting Widgets Instantiation.\n");
  instantiateWidgets();
LV_LOG("Instantiation of widgets complete.\n");
    /* Initialize your hardware. */
    instantiateCommonItems();
LV_LOG("Instantiation of common items complete.\n");

  plvTask = new LVTaskHandler();
  assert(plvTask);

}

void loop() {
    /* Create the UI or start a task for it.
     * In the end, don't forget to call `lv_task_handler` in a loop. */

// - now handled by plvTask -    
//  lv_task_handler();

}
