# LVGLPlusPlus Threading Example

There are a pair of sample programs to demonstrate [LVGLPlusPlus](https://github.com/bobwolff68/LVGLPlusPlus) C++ library:
- A threaded version [LVGLPlusPlus-ThreadedSample](https://github.com/bobwolff68/LVGLPlusPlus-ThreadedSample)
- A non-threaded version [LVGLPlusPlus-NonThreadedSample](https://github.com/bobwolff68/LVGLPlusPlus-NonThreadedSample)

The pair of examples (with and without threading) are intended to both educate on how to use some of the LVGLPlusPlus constructs as well as to be used as "starting templates" for those who wish to have a known good setup that functions in these configurations.

The examples currently are functional on Mac in so-called 'native' or 'simulation' compilation and utilize SDL2 and lv_drivers to do the screen/graphics platform support. The examples are also functional on an ESP32 DevKit circa 2021 attached to a 2.4" TFT touchscreen display based on the ILI9341 and this utilizes the Bodmer library "TFT_eSPI" (which is excellent). The IO pins for the TFT controls must be gleaned from the platform.ini file. Your connections may be completely different from these.

## Important Resources at the top level
- The library upon which these samples site is [LVGLPlusPlus](https://bobwolff68.github.io/LVGLPlusPlus)
- The full Doxygen-generated docs for the LVGLPlusPlus library can be found on my Github Pages at [LVGLPlusPlus Doxygen Docs](https://bobwolff68.github.io/LVGLPlusPlus)
- If you're unfamiliar with LVGL, please go check out the [Main LVGL Site](https://lvgl.io)
  - The interactive forum for LVGL is also an excellent resource. Go check it out at [LVGL Forum](https://forum.lvgl.io/)
  - The main github repo for LVGL itself is at [LVGL Main](https://github.com/lvgl/lvgl).
  - The PlatformIO example on LVGL is at [LVGL with PlatformIO](https://github.com/lvgl/lv_platformio)
- The build environment I use is Microsoft VSCode with the PlatformIO extension. This library is a PlatformIO library.
- I'll be turning on the Github _Issues_ and _Discussions_ features as soon as I can after going live with the public repo.
- Feel free to email me - b o b dot w o l f f 6 8 at g m a i l dot com - happy to entertain questions.

## To Do Items

- Once the main library is public, it will be added to the platformio registry. At that point, the platformio.ini file needs to be updated to no longer point directly at the github URL but instead properly reference the pio registry lib_deps library. This will make auto-upgrading seamless.
  - Currently, if the main library is updated and you've already built the sample project, there is no way for the sample project to know of the update and the (clunky) method to forcing this to be pulled again is by running a helper script called `./cleanLib.sh` which simply removes the underlying library from the .pio/libdeps/* folders. PlatformIO's library registry actually uses the library.json manifest to know when updates are made and compares these as part of the build process pre-build.
- Document the SDL bit on the Mac (homebrew and associated platform.ini items)
- Document use of lv_drivers in LVGLPlusPlus and how/why it is present in esp32 build

## Looking for digital engineering design services?

- As of Spring 2023 I'm open to contract work and happy to discuss projects. Get in touch - b o b dot w o l f f 6 8 at g m a i l dot c o m
