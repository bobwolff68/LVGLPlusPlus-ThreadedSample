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
#pragma once

#include "lvpp.h"
#include <vector>
#include "GlobalObjects.h"

void instantiateWidgets(void);

class PlusTime : public lvppButton {
public:
    PlusTime(void);
    virtual ~PlusTime();
protected:
    void onClicked(void);
};

class SetupControl : public lvppButton {
public:
    SetupControl(void);
    virtual ~SetupControl();
    void onClicked(void);
};

class TimeStatus : public lvppButton {
public:
    TimeStatus(void);
    virtual ~TimeStatus();
    void setText(const char* pText);
protected:
    lv_style_t style_shadow;
    lv_obj_t* shadow_label;
};

class FullnessBar : public lvppBar {
public:
    FullnessBar(void);
    virtual ~FullnessBar();
};

class H2OFullnessLabel : public lvppLabel {
public:
    H2OFullnessLabel(void);
    virtual ~H2OFullnessLabel();
};

class LEDControl : public lvppCycleButton {
public:
    LEDControl(void);
    virtual ~LEDControl();

    enum class LEDStyle : uint8_t { Off, SlowCycle, FastCycle, Walking, Jogging };
    std::vector<std::string> styles;

    void onClicked();
    LEDStyle getCurrentStyle(void);
    const char* getCurrentStyleString(void);
protected:
    LEDStyle currentStyle;
};

class LEDLabel : public lvppLabel {
public:
    LEDLabel(void);
    virtual ~LEDLabel();
};

class CycleControl : public lvppDropdown {
public:
    CycleControl();
    virtual ~CycleControl();
    void onValueChanged();
protected:
    enum class CycleStyle : uint8_t { AllOn, OneSec, TwoSec, ThreeSec };
    std::vector<std::string> styles { LV_SYMBOL_REFRESH " All On", LV_SYMBOL_REFRESH " 2 Secs", LV_SYMBOL_REFRESH " 3 Secs", LV_SYMBOL_REFRESH " 4 Secs" };
    CycleControl::CycleStyle curIndex;
};

class CPLabel : public lvppLabel {
public:
    CPLabel(void);
    virtual ~CPLabel();
};

class BackgroundAreas : public lvppCanvas {
public:
    BackgroundAreas(void);
    virtual ~BackgroundAreas();
};


class TempGauge : public lvppArc {
public:
    TempGauge(void);
    virtual ~TempGauge();
    void onValueChanged();
    void enableLabel(const char* pText);
    void setTemp(uint8_t tempValue);
protected:
    lv_obj_t* pLabel;
    lv_style_t labelStyle;
};
