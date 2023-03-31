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
#include "TheBrain.h"
#include "Widgets.h"

#ifndef ESP_PLATFORM
int16_t getrand(int16_t low, int16_t high) {
    int totalRange = high-low;
    float v = rand()/RAND_MAX;  // Gives back 0-1 in float
    int randRange = v*totalRange;   // Now we're normalized to the desired range
    int final = low + randRange;
    printf("normalized 0-1 is: %f, randRange: %d, and final: %d\n", v, randRange, final);
    return final;
}

int16_t constrain(int16_t v, int16_t low, int16_t high) {
    return v < low ? low : (v > high ? high : v);
}
#endif


void instantiateCommonItems() {
   // TheBrain needs created 'somewhere' in this simulation
   pTheBrain = new TheBrain();
}

TheBrain::TheBrain() : LockingRoboTask("TheBrain", 1, 8192) {
    temperature = 62;
    secondsRemaining = 60;
    fullPercentage = 40;

    assert(pTempGauge);
    assert(pFull);
    assert(pTimeStatus);

    setBaseRunDelay(200);
    this->Start();
}

TheBrain::~TheBrain() {
}

void TheBrain::Run() {
    if (hasElapsed(1000)) {
        resetElapsedTimer();

        // Update UI items
        temperature += 5;
        if (temperature > 105)
            temperature = 55;
        pTempGauge->setTemp(temperature);

        fullPercentage += 5;
        if (fullPercentage > 100)
            fullPercentage = 10;
        pFull->setValue(fullPercentage);

        secondsRemaining--;
        if (secondsRemaining <= 0) {
            pTimeStatus->setText("STOPPED");
            secondsRemaining = 0;
        }
        else {
            std::string rem("Time: " + std::to_string(secondsRemaining));
            pTimeStatus->setText(rem.c_str());
        }
    }

}

void TheBrain::AddSeconds(uint16_t secs) {
    secondsRemaining += secs;
}
