/*******************************
 * 
 * FILE ONLY GETS COMPILED IN EMULATION MODE FOR THE PC/MAC
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

#include "lvpp.h"

#include "main_header.h"
#include "Widgets.h"

extern lv_obj_t* pSetupScreen;
extern lv_obj_t* pMainScreen;
extern void instantiateCommonItems();

int main(void)
{
	lv_init();

	hal_setup();

   lv_theme_t * th = lv_theme_default_init(NULL, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_BLUE_GREY), 
                                                false, LV_FONT_DEFAULT);

    lv_disp_set_theme(NULL, th); /*Assign the theme to the display*/

LV_LOG_USER("Ready to create widgets.\n");
    
    instantiateWidgets();

LV_LOG("Created widgets.\n");

    instantiateCommonItems();

LV_LOG("Instantiation of common items complete.\n");

//	hal_loop();
// Final loop with the ability to add our own stuff in there.
    while(1) {
        hal_delay();
        // If you're running task-based UI, you'll need this mutex and the associated UI tasks will be of type LockingRoboTask.
        LockingRoboTask::TakeMutex();
        lv_task_handler();
        LockingRoboTask::GiveMutex();

    // Can do other emulated work here.
    }
}
