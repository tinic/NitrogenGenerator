/* Copyright 2025 Tinic Uro

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _SCREEN_H_
#define _SCREEN_H_

#ifdef __cplusplus
extern "C" {
#endif  // #ifdef __cplusplus
void screen_init();
#ifdef __cplusplus
};
#endif  // #ifdef __cplusplus

#ifdef __cplusplus

#include <cstdint>
#include "font.h"

class ST7525 {
    public:
     static ST7525 &instance();

     static constexpr uint8_t COLUMNS = 192;
     static constexpr uint8_t LINES = 64;
     static constexpr uint8_t PAGES = LINES / 8;
 
     static constexpr uint8_t CMD_SET_COLUMN_LSB = 0x00;
     static constexpr uint8_t CMD_SET_COLUMN_MSB = 0x10;
     static constexpr uint8_t CMD_SET_SCROLL_LINE = 0x40;
     static constexpr uint8_t CMD_SET_PAGE_ADRESS = 0xB0;
     static constexpr uint8_t CMD_SET_CONTRAST = 0x81;
     static constexpr uint8_t CMD_SET_PARTIAL_SCREEN = 0x84;
     static constexpr uint8_t CMD_SET_RAM_ADDR_CTRL = 0x88;
     static constexpr uint8_t CMD_SET_FRAME_RATE = 0xA0;
     static constexpr uint8_t CMD_SET_ALL_PIXEL_ON = 0xA4;
     static constexpr uint8_t CMD_SET_INVERSE_DISPLAY = 0xA5;
     static constexpr uint8_t CMD_SET_DISPLAY_EN = 0xAE;
     static constexpr uint8_t CMD_SET_SCAN_DIR = 0xC0;
     static constexpr uint8_t CMD_SOFT_RESET = 0xE2;
     static constexpr uint8_t CMD_SET_BIAS = 0xE4;
     static constexpr uint8_t CMD_SET_COM_END = 0xF1;
     static constexpr uint8_t CMD_SET_PART_START_ADDR = 0xF2;
     static constexpr uint8_t CMD_SET_PART_END_ADDR = 0xF3;
     static constexpr uint8_t CMD_SET_TEST_CONTROL = 0xF0;
     static constexpr uint8_t CMD_NOP = 0xE3;
 
     void init();
     void update();
 
    private:
     uint8_t framebuffer[PAGES * COLUMNS] = {0xFF};

     bool initialized = false;
 
     void set_pixel(int32_t x, int32_t y, uint8_t c);
     void draw_char(int32_t x, int32_t y, const CharInfo &ch, const uint8_t *data);
     int32_t draw_string(int32_t x, int32_t y, const char *str, bool calcWidthOnly = false);

     void clear();
     void write_frame();

     void send_cmd(uint8_t v);
     void send_dat(uint8_t v);
 };
 #endif  // #ifdef __cplusplus

#endif  // #ifndef _SCREEN_H_
