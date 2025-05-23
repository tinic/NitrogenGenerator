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
#include "./screen.h"

#include <cstdio>

#ifndef SCREEN_TEST
#include "./Core/Inc/main.h"
#endif  // #ifndef SCREEN_TEST

#include "./mcp.h"
#ifndef SCREEN_TEST
#include "boot.h"
#include "version.h"
#endif  // #ifndef SCREEN_TEST

// clang-format off
#include "constixel/constixel.hpp"
#include "ibmplexsans_regular_18_mono.hpp"
using font = constixel::ibmplexsans_regular_18_mono;
// clang-format on

#ifndef SCREEN_TEST
extern "C" SPI_HandleTypeDef hspi1;
#endif  // #ifndef SCREEN_TEST

using image_type = constixel::image<constixel::format_1bit, 192, 64>;
static image_type screen;

using buffer_type = constixel::image<constixel::format_1bit, 64, 192>;
static buffer_type buffer;

ST7525 &ST7525::instance() {
    static ST7525 st7525display;
    if (!st7525display.initialized) {
        st7525display.initialized = true;
        st7525display.init();
    }
    return st7525display;
}

void ST7525::init() {
#ifndef SCREEN_TEST
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_Delay(1);

    HAL_GPIO_WritePin(GPIOA, LCD_RESET_Pin, GPIO_PIN_SET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(GPIOA, LCD_RESET_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(GPIOA, LCD_RESET_Pin, GPIO_PIN_SET);

    send_cmd(CMD_SOFT_RESET);
    HAL_Delay(2);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
#endif  // #ifndef SCREEN_TEST

    send_cmd(0xA0);  // Frame Rate
    send_cmd(0xEB);  // BR
    send_cmd(0x2F);  // Power Control
    send_cmd(0xC4);  // LCD Mapping
    send_cmd(0x81);  // Output Voltage
    send_cmd(154);   // Voltage Level
    send_cmd(0xAF);  // Display Enable
}

void ST7525::clear() {
    screen.clear();
}

void ST7525::set_boot() {
#ifndef SCREEN_TEST
    screen.copy<sizeof(boot_bitmap_data)>(boot_bitmap_data);
#endif  // #ifndef SCREEN_TEST
}

void ST7525::write_frame() {
    send_cmd(CMD_SET_PAGE_ADRESS);
    send_cmd(CMD_SET_COLUMN_MSB);
    send_cmd(CMD_SET_COLUMN_LSB);

    screen.transpose(buffer);
    for (auto c : buffer.data_ref()) {
        send_dat(c);
    }
}

#ifdef SCREEN_TEST
void ST7525::output() {
    screen.sixel_to_cout<4>();
}
#endif  // #ifdef SCREEN_TEST

void ST7525::update() {
    clear();

    static char output[64] = {};
    if (MCP::instance().SystemTime() < 5) {
        set_boot();
    } else if (MCP::instance().SystemTime() < 10) {
        snprintf(output, sizeof(output), "www.aeron2.com");
        screen.text_centered_mono<font>(192 / 2, 0, output).color(1);
        snprintf(output, sizeof(output), "build %s", 
#ifndef SCREEN_TEST
            GIT_REV_COUNT
#else
            "test"
#endif
        );
        screen.text_centered_mono<font>(192 / 2, 20, output).color(1);
        snprintf(output, sizeof(output), "%s", 
#ifndef SCREEN_TEST
            GIT_COMMIT_DATE_SHORT
#else
            "test-date"
#endif
        );
        screen.text_centered_mono<font>(192 / 2, 40, output).color(1);
    } else {
        snprintf(output, sizeof(output), "􀇤");
        screen.text_mono<font>(0, -2, output).color(1);
        snprintf(output, sizeof(output), "%dpsi", static_cast<int>(MCP::instance().PSI0()));
        screen.text_mono<font>(192 / 2 - screen.string_width<font>(output) - 4, -2, output).color(1);
        snprintf(output, sizeof(output), "N²");
        screen.text_mono<font>(192 / 2 + 4, -2, output).color(1);
        snprintf(output, sizeof(output), "%dpsi", static_cast<int>(MCP::instance().PSI1()));
        screen.text_mono<font>(191 - screen.string_width<font>(output), -2, output).color(1);

        snprintf(output, sizeof(output), "%s", MCP::instance().Solenoid0() ? "Open" : "Clsd");
        screen.text_mono<font>(0, 18, output).color(1);
        snprintf(output, sizeof(output), "%s", MCP::instance().Solenoid1() ? "Open" : "Clsd");
        screen.text_mono<font>(192 / 2 + 4, 18, output).color(1);

        screen.line(192 / 2 + 1, 0, 192 / 2 + 1, 42).stroke(1);
        screen.line(0, 42, 192, 42).stroke(1);

        snprintf(output, sizeof(output), "%02d%%", static_cast<int>(MCP::instance().DutyCycleAverage(0) * 100.0f));
        screen.text_mono<font>(192 / 2 - screen.string_width<font>(output) - 4, 18, output).color(1);
        snprintf(output, sizeof(output), "%02d%%", static_cast<int>(MCP::instance().DutyCycleAverage(1) * 100.0f));
        screen.text_mono<font>(191 - screen.string_width<font>(output), 18, output).color(1);

        const int32_t h = (static_cast<int32_t>(MCP::instance().SystemTime()) / 3600);
        const int32_t m = (static_cast<int32_t>(MCP::instance().SystemTime()) / 60) % 60;
        const int32_t s = (static_cast<int32_t>(MCP::instance().SystemTime())) % 60;
        snprintf(output, sizeof(output), "􀐫%03d:%02d:%02d", static_cast<int>(h), static_cast<int>(m), static_cast<int>(s));
        screen.text_mono<font>(0, 42, output).color(1);

        // 0x1001E4, 0x10042B, 0x10042F, 0x000B2, 0x26A0

        if (MCP::instance().FaultState()) {
            snprintf(output, sizeof(output), "⚠Fault!");
            screen.text_mono<font>(191 - screen.string_width<font>(output), 42, output).color(1);
        } else {
            const int32_t em = (static_cast<int32_t>(MCP::instance().RefillElapsedTime()) / 60) % 60;
            const int32_t es = (static_cast<int32_t>(MCP::instance().RefillElapsedTime())) % 60;
            snprintf(output, sizeof(output), "􀐯%03d:%02d", static_cast<int>(em), static_cast<int>(es));
            screen.text_mono<font>(191 - screen.string_width<font>(output), 42, output).color(1);
        }
    }
    write_frame();
}

void ST7525::send_cmd(uint8_t v) {
#ifndef SCREEN_TEST
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, LCD_CD_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &v, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
#endif  // #ifndef SCREEN_TEST
}

void ST7525::send_dat(uint8_t v) {
#ifndef SCREEN_TEST
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, LCD_CD_Pin, GPIO_PIN_SET);
    HAL_SPI_Transmit(&hspi1, &v, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
#endif  // #ifndef SCREEN_TEST
}

void screen_init() {
    ST7525::instance().init();
}
