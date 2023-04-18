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



////////////////////////////////////////
//
//  instantiateWidgets
//
////////////////////////////////////////

void instantiateWidgets(void) {
    pScreenMain = new lvppScreen(lv_scr_act());

    static lvppCanvasIndexed bground("back2", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 4);
    const lv_coord_t topYarea = 117;
    const lv_coord_t botXdivider = 158;

    bground.addColorToIndex(lv_color_white());
    bground.addColorToIndex(lv_color_black());
    bground.addPaletteToIndex(LV_PALETTE_BLUE_GREY);

    bground.setbgColor(lv_palette_lighten(LV_PALETTE_BLUE_GREY, 1));
    bground.drawLineHoriz(0,topYarea, SCREEN_WIDTH, lv_color_black());
    bground.drawLineVert(botXdivider, topYarea, SCREEN_HEIGHT-topYarea, lv_color_black());

    pScreenMain->addObject(&bground);

    static lvppButton setupButton("Setup", LV_SYMBOL_SETTINGS" Setup");
    setupButton.align(LV_ALIGN_BOTTOM_RIGHT, -3, -3);
    setupButton.setCallbackOnClicked([]() -> void {
        // Time to load the setup screen.
        if (pScreenSetup) {
            pScreenSetup->activateScreen(500, LV_SCR_LOAD_ANIM_OVER_LEFT);
        }
    });

    pScreenMain->addObject(&setupButton);

    static lvppBar fullnessBar("H2OLevel");
    fullnessBar.setSize(13, 80);
    fullnessBar.align(LV_ALIGN_TOP_RIGHT, -19, 20);
    fullnessBar.setRange(0,100);
    fullnessBar.setValueLabelFormat("%d%%");
    fullnessBar.enableValueLabel(2, 17, LV_ALIGN_BOTTOM_MID);
    fullnessBar.setValue(20);
    fullnessBar.setAdjText("Water", 0, -50);

    pScreenMain->addObject(&fullnessBar);

    printf("size of buffer for TRUE_COLOR:%d\n", LV_CANVAS_BUF_SIZE_TRUE_COLOR(320,240));
    printf("size of buffer for INDEXED 8-bit:%d\n", LV_CANVAS_BUF_SIZE_INDEXED_8BIT(320,240));
    printf("size of buffer for INDEXED 4-bit:%d\n", LV_CANVAS_BUF_SIZE_INDEXED_4BIT(320,240));

    static lvppButton plus5("+5Min", "  Add\n+1 Min");
    plus5.setSize(70, 55);
    plus5.align(LV_ALIGN_TOP_MID, 0, 7);
    plus5.setFontSize(20);
    plus5.setBGColor(lv_palette_lighten(LV_PALETTE_GREEN, 1));
    plus5.setCallbackOnClicked([]() -> void {
        assert(pTheBrain);
        pTheBrain->AddSeconds(60);
    });

    pScreenMain->addObject(&plus5);

    static lvppCycleButton lights("Lights");
    lights.setSize(61, 28);
    lights.align(LV_ALIGN_TOP_LEFT, 6, 35);
    lights.addOptions(LV_SYMBOL_POWER " Off");
    lights.addOptions(LV_SYMBOL_SHUFFLE " Slow");
    lights.addOptions(LV_SYMBOL_SHUFFLE " Fast");
    lights.setAdjText("Lights", 0, -24);

    pScreenMain->addObject(&lights);

    static lvppDropdown dropCycle("DropCycle");
    dropCycle.setSize(148, 42);
    dropCycle.align(LV_ALIGN_BOTTOM_LEFT, 5, -40);
    dropCycle.setFontSize(22);
    dropCycle.setBGColor(lv_palette_darken(LV_PALETTE_BLUE, 1));
    dropCycle.setOptions(LV_SYMBOL_REFRESH " All On\n" LV_SYMBOL_REFRESH " 2 Secs\n" LV_SYMBOL_REFRESH " 3 Secs\n" LV_SYMBOL_REFRESH " 4 Secs");
    dropCycle.setDropdownDirection(LV_DIR_TOP);
    dropCycle.setAdjText("Cycle Pulsing", 0, -32);

    pScreenMain->addObject(&dropCycle);

    pTimeStatus = new TimeStatus;
    pScreenMain->addObject(pTimeStatus);

    pTempGauge = new TempGauge;
    pScreenMain->addObject(pTempGauge);

////////////////////////////////////////
//
//  SETUP SCREEN ITEMS - JUST A FEW
//
////////////////////////////////////////
    pScreenSetup = new lvppScreen();
    
    static lvppLabel hello("title", "Setup Screen Example");
    hello.align(LV_ALIGN_TOP_MID, 0, 3);

    pScreenSetup->addObject(&hello);

    static lvppButton exitSetupButton("ExitSetup", "Exit");
    exitSetupButton.align(LV_ALIGN_BOTTOM_RIGHT, -3, -3);
    exitSetupButton.setCallbackOnClicked([]() -> void {
        // Time to load the main screen again.
        pScreenMain->activateScreen(500, LV_SCR_LOAD_ANIM_OVER_RIGHT);
    });

    pScreenSetup->addObject(&exitSetupButton);

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
//  T e m p G a u g e
//
////////////////////////////////////////

TempGauge::TempGauge() : lvppArc("Temp") {
    align(LV_ALIGN_BOTTOM_RIGHT, -47, -7);
    setSize(105, 105);

    // Temps ranging from 60 to 105
    setRange(60, 105);
    setArcRotationAndSweep(100, 0, 200);

    lv_obj_remove_style(obj, NULL, LV_PART_KNOB);
//    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);

    enableValueLabel(-20, -10);

    setValueLabelFont(&lv_font_montserrat_32);
    setValueLabelFormat("%d F");
    setTemp(65);
}

void TempGauge::onValueChanged() {
    curValue = lv_arc_get_value(obj);
    // hue range is 0-359
    // saturation and value are 0-100
    uint16_t h;

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

    setValueLabelColor(tempColor);
    setArcColor(tempColor);
}

void TempGauge::setTemp(uint8_t tempValue) {
    setValue(tempValue);
}
