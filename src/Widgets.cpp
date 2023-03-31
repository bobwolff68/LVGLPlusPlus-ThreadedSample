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
#include "main_header.h"
#include "Widgets.h"

lv_obj_t* pSetupScreen=nullptr;
lv_obj_t* pMainScreen=nullptr;


////////////////////////////////////////
//
//  SETUP SCREEN ITEMS - JUST A FEW
//
////////////////////////////////////////

class ExitSetupButton : public lvppButton {
public:
    ExitSetupButton(lv_obj_t* parent=nullptr) : lvppButton("S-exit", "Exit", parent) {
        align(LV_ALIGN_BOTTOM_RIGHT, -3, -3);
    };
protected:
    void onClicked(void) {
        // Time to swap out my screen for my parent.
        lv_scr_load_anim(pMainScreen, LV_SCR_LOAD_ANIM_OUT_RIGHT, 500, 0, false);
   }
};

class HelloLabel : public lvppLabel {
public:
    HelloLabel(lv_obj_t* parent=nullptr) : lvppLabel("Hello", "Setup Screen", parent) {
        align(LV_ALIGN_TOP_MID, 0, 3);
        setText("Setup Screen Example");
    };
};



////////////////////////////////////////
//
//  instantiateWidgets
//
////////////////////////////////////////

void instantiateWidgets(void) {
    // WARNING: Be sure to do this PRIOR to anyone calling lv_scr_load()
    pMainScreen = lv_scr_act();

    printf("size of buffer for TRUE_COLOR:%d\n", LV_CANVAS_BUF_SIZE_TRUE_COLOR(320,240));
    printf("size of buffer for INDEXED 8-bit:%d\n", LV_CANVAS_BUF_SIZE_INDEXED_8BIT(320,240));
    printf("size of buffer for INDEXED 4-bit:%d\n", LV_CANVAS_BUF_SIZE_INDEXED_4BIT(320,240));

    static BackgroundAreas background;

    pFull = new FullnessBar;
    static H2OFullnessLabel waterLabel;

    static PlusTime plus5;

    pTimeStatus = new TimeStatus;

    static LEDControl leds;
    static LEDLabel ledLabel;

    pCycleControl = new CycleControl;
    static CPLabel cpLabel;

    pTempGauge = new TempGauge;
    static SetupControl setup;

//
// SETUP SCREEN
//
    pSetupScreen = lv_obj_create(NULL);
    
    static ExitSetupButton exit(pSetupScreen);
    static HelloLabel hello(pSetupScreen);
}

PlusTime::PlusTime(void) : lvppButton("+5Min", "  Add\n+1 Min") {
    setSize(70, 55);
    align(LV_ALIGN_TOP_MID, 0, 7);
    setFontSize(20);

    lv_style_set_bg_color(&style_obj, lv_palette_lighten(LV_PALETTE_GREEN, 1));
    lv_obj_add_style(obj, &style_obj, 0);

}

PlusTime::~PlusTime() {

}

void PlusTime::onClicked(void) {
    assert(pTheBrain);
    pTheBrain->AddSeconds(60);
}



////////////////////////////////////////
//
//  M i s t i n g S t a t u s
//
////////////////////////////////////////

TimeStatus::TimeStatus(void) : lvppButton("MistStatus", "Remaining:") {
    setSize(150, 44);
    align(LV_ALIGN_CENTER, 0, -32);
//    setFontSize(32);

    setTextColor(lv_palette_darken(LV_PALETTE_GREEN, 4));
    lv_style_set_bg_color(&style_obj, lv_palette_lighten(LV_PALETTE_GREEN, 2));
    lv_style_set_border_color(&style_obj, lv_palette_darken(LV_PALETTE_GREEN, 3));
//    lv_style_set_text_color(&style_obj, lv_palette_darken(LV_PALETTE_GREEN, 4));
    lv_obj_add_style(obj, &style_obj, 0);

    lv_style_init(&style_shadow);
    lv_style_set_text_opa(&style_shadow, LV_OPA_40);
    lv_style_set_text_color(&style_shadow, lv_color_black());

    shadow_label = lv_label_create(lv_scr_act());
    lv_obj_add_style(shadow_label, &style_shadow, 0);

    lv_label_set_text(shadow_label, lv_label_get_text(label));
    lv_obj_align_to(shadow_label, label, LV_ALIGN_TOP_LEFT, 1, 1);

    // Setup the main font and the shadow behind it.
    lv_style_set_text_font(&style_obj, &lv_font_montserrat_24);
    lv_obj_add_style(label, &style_obj, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_style_set_text_font(&style_shadow, &lv_font_montserrat_24);
    lv_obj_add_style(shadow_label, &style_shadow, LV_PART_MAIN | LV_STATE_DEFAULT);
}

TimeStatus::~TimeStatus() {

}

void TimeStatus::setText(const char* pText) {
    if (pText) {
        lv_label_set_text(label, pText);
        lv_label_set_text(shadow_label, pText);
    }
}



////////////////////////////////////////
//
//  L E D C o n t r o l
//
////////////////////////////////////////

LEDControl::LEDControl() : lvppCycleButton("LEDControl") {
    setSize(61, 28);
    align(LV_ALIGN_OUT_LEFT_TOP, 4, 25);

    styles.push_back(LV_SYMBOL_POWER " Off");
    styles.push_back(LV_SYMBOL_SHUFFLE " Slow");
    styles.push_back(LV_SYMBOL_SHUFFLE " Fast");

    clearOptions();
    addOptionsFromVector(styles);

    currentIndex = (uint16_t)0;
    setText(options[currentIndex].c_str());

    printf("LEDControl constructor: currentStyle is %d\n", (uint8_t)currentStyle);
}

LEDControl::~LEDControl() {

}

void LEDControl::onClicked() {
//        setText("Dance");
printf("LEDControl::onClicked has been called.\n");
    currentStyle = (LEDStyle)currentIndex;
    printf("LEDControl onClicked: currentStyle is %d\n", (uint8_t)currentStyle);
}

LEDControl::LEDStyle LEDControl::getCurrentStyle(void) {
    return currentStyle;
}

const char* LEDControl::getCurrentStyleString(void) {
    return styles[int(currentStyle)].c_str();
}

LEDLabel::LEDLabel() : lvppLabel("LEDLabel", "Lights") {
    align(LV_ALIGN_OUT_LEFT_TOP, 9, 4);
}

LEDLabel::~LEDLabel() {

}


////////////////////////////////////////
//
//  C y c l e C o n t r o l
//
////////////////////////////////////////

CycleControl::CycleControl() : lvppDropdown("CycleControl") {
    setSize(148, 42);
    align(LV_ALIGN_BOTTOM_LEFT, 5, -38);
    setFontSize(22);

    lv_style_set_pad_ver(&style_obj, 8);
    lv_obj_add_style(obj, &style_obj, 0);
    setBGColor(lv_palette_darken(LV_PALETTE_BLUE, 1));

    setOptions(styles);
    setDropdownDirection(LV_DIR_TOP);
    setCurrentIndex((uint16_t)0);
}

CycleControl::~CycleControl() {

}

void CycleControl::onValueChanged() {
    curIndex = (CycleStyle) getCurrentIndex();
}


CPLabel::CPLabel() : lvppLabel("CPLabel", "Rotation Period") {
    align(LV_ALIGN_BOTTOM_LEFT, 21, -102);
}

CPLabel::~CPLabel() {

}


////////////////////////////////////////
//
//  S e t u p C o n t r o l
//
////////////////////////////////////////

SetupControl::SetupControl() : lvppButton("Setup", LV_SYMBOL_SETTINGS" Setup") {
    align(LV_ALIGN_BOTTOM_RIGHT, -3, -3);
}

SetupControl::~SetupControl() {

}

void SetupControl::onClicked() {
    // Time to load the setup screen.
    if (pSetupScreen) {
        lv_scr_load_anim(pSetupScreen, LV_SCR_LOAD_ANIM_OVER_LEFT, 500, 0, false);
    }
}


////////////////////////////////////////
//
//  H 2 O F u l l n e s s B a r
//
////////////////////////////////////////

FullnessBar::FullnessBar(void) : lvppBar("H2OLevel") {
    setSize(13, 80);
    align(LV_ALIGN_TOP_RIGHT, -19, 20);
    setRange(0,100);

    setLabelFormat("%d%%");
    enableLabel(LV_ALIGN_BOTTOM_MID, 3, 17);
    setValue(20);
}

FullnessBar::~FullnessBar() {

}

H2OFullnessLabel::H2OFullnessLabel() : lvppLabel("H2OLabel", "Water") {
    align(LV_ALIGN_TOP_RIGHT, -5, 3);
}

H2OFullnessLabel::~H2OFullnessLabel() {

}


////////////////////////////////////////
//
//  T e m p G a u g e
//
////////////////////////////////////////

TempGauge::TempGauge() : lvppArc("Temp") {
    align(LV_ALIGN_BOTTOM_RIGHT, -47, -7);
    setSize(105, 105);

    // Temps ranging from 60 to 105
    lv_arc_set_range(obj, 60, 105);

    lv_arc_set_rotation(obj, 100);
    lv_arc_set_bg_angles(obj, 0, 200);
    lv_obj_remove_style(obj, NULL, LV_PART_KNOB);
//    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);

    pLabel = lv_label_create(obj);
    lv_obj_align_to(pLabel, obj, LV_ALIGN_CENTER, -20, -10);

    lv_style_set_text_font(&style_obj, &lv_font_montserrat_32);
    lv_obj_add_style(obj, &style_obj, LV_PART_MAIN | LV_STATE_DEFAULT);

    curValue = 65;
    lv_arc_set_value(obj, curValue);
    lv_label_set_text_fmt(pLabel, "%d F", curValue);
    onValueChanged();
}

TempGauge::~TempGauge() {
    if (pLabel) {
        delete pLabel;
        pLabel = nullptr;
    }
}

void TempGauge::onValueChanged() {
    curValue = lv_arc_get_value(obj);
    // hue range is 0-359
    // saturation and value are 0-100
    uint16_t h;

    lv_label_set_text_fmt(pLabel, "%d F", curValue);

    // 0 = reddest
    // 20 = beginning orange
    // 30 - deep orange
    // 60 - deep yellow
    // 120 - deep green
    // 180 - green/blue
    // 200 - light blue
    // 220 - dark blue

    if (curValue >= 95)
        h = 5;
    else if (curValue >= 80)
        h = 30;
    else if (curValue >= 70)
        h = 50;
    else
        h = 200;

    lv_color_t tempColor = lv_color_hsv_to_rgb(h, 100, 100);

//    lv_style_set_arc_color(&style_obj, tempColor);
    lv_style_set_text_color(&style_obj, tempColor);
//    lv_obj_add_style(obj, &style_obj, 0);

}

void TempGauge::setTemp(uint8_t tempValue) {
    setValue(tempValue);
    onValueChanged();
}


////////////////////////////////////////
//
//  B a c k g r o u n d A r e a s
//
////////////////////////////////////////

BackgroundAreas::BackgroundAreas(void) : lvppCanvas("Backgnd", 0, 0, SDL_HOR_RES, SDL_VER_RES) {
    const lv_coord_t topYarea = 117;
    const lv_coord_t botXdivider = 158;

    // Top area
    lv_coord_t x, y;
    lv_color_t px;
    px.full = 1;
    
    lv_canvas_fill_bg(obj, px, LV_OPA_COVER);

    // Black index
    px.full = 0;
    // Draw horizontal dividing line
    for (x = 0; x<320; x++)
        for (y=topYarea; y<(topYarea+4); y++)
            lv_canvas_set_px_color(obj, x, y, px);

    // Draw vertical divider in lower half of display
    for (x = botXdivider; x<botXdivider+4; x++)
        for (y=topYarea; y<240; y++)
            lv_canvas_set_px_color(obj, x, y, px);
}

BackgroundAreas::~BackgroundAreas() {

}

