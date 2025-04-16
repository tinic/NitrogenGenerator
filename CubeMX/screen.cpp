#include "screen.h"

#include <cstdio>
#include <cstring>

#include <fixed_containers/fixed_map.hpp>

#include "font_0.h"
#include "font.h"
#include "Core/Inc/main.h"

extern "C" ADC_HandleTypeDef hadc1;

extern "C" SPI_HandleTypeDef hspi1;

extern "C" UART_HandleTypeDef huart1;

extern "C" TIM_HandleTypeDef htim1;

extern "C" TIM_HandleTypeDef htim3;

class ST7525Display {
   public:
    static ST7525Display &instance() {
        static ST7525Display st7525display;
        if (!st7525display.initialized) {
            st7525display.initialized = true;
            st7525display.init();
        }
        return st7525display;
    }

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

    void init() {
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

        send_cmd(0xA0);  // Frame Rate
        send_cmd(0xEB);  // BR
        send_cmd(0x2F);  // Power Control
        send_cmd(0xC4);  // LCD Mapping
        send_cmd(0x81);  // Output Voltage
        send_cmd(154);   // Voltage Level
        send_cmd(0xAF);  // Display Enable
    }

    void set_pixel(uint8_t x, uint8_t y, uint8_t c) {
        if (x >= COLUMNS || y >= LINES)
            return;
        uint16_t idx = (y / 8) * COLUMNS + x;
        if (c)
            framebuffer[idx] |= (1 << (y % 8));
        else
            framebuffer[idx] &= ~(1 << (y % 8));
    }

    void draw_char(int32_t x, int32_t y, const CharInfo &ch, const uint8_t *data) {
        const size_t stride = (IMAGE_ASSET_WIDTH + 7) & ~(7);
        data += (ch.y * stride + ch.x) / 8;
        x += ch.xoffset;
        y += ch.yoffset;
        int32_t x2 = x + int32_t(ch.width);
        int32_t y2 = y + int32_t(ch.height);
        for (int32_t yy = y; yy < y2; yy++) {
            for (int32_t xx = x; xx < x2; xx++) {
                int x_off = (xx - x) + (ch.x % 8);
                int byte_index = x_off / 8;
                int bit_index = 7 - (x_off % 8);
                uint8_t a = (data[byte_index] >> bit_index) & 1;
                if (!a) {
                    set_pixel(xx, yy, 1);
                }
            }
            data += stride / 8;
        }
    
    }

    int32_t draw_string(int32_t x, int32_t y, const char *str, bool calcWidthOnly = false) {
        uint32_t lastCodePoint = 0;
        const CharInfo *chInfo = 0;
        const uint8_t *chData = 0;
        while (*str != '\0') {
            uint32_t codePoint = 0;
            unsigned char lead = *str;
            if (lead < 0x80) {
                // Single byte (ASCII)
                codePoint = lead;
                str += 1;
            } else if ((lead >> 5) == 0x06) {
                // Two bytes
                codePoint = ((lead & 0x1F) << 6) | (str[1] & 0x3F);
                str += 2;
            } else if ((lead >> 4) == 0x0E) {
                // Three bytes
                codePoint = ((lead & 0x0F) << 12) | ((str[1] & 0x3F) << 6) | (str[2] & 0x3F);
                str += 3;
            } else if ((lead >> 3) == 0x1E) {
                // Four bytes
                codePoint = ((lead & 0x07) << 18) | ((str[1] & 0x3F) << 12) | ((str[2] & 0x3F) << 6) | (str[3] & 0x3F);
                str += 4;
            } else {
                return 0;
            }

            if (FONT_KERNINGS_COUNT) {
                x += FONT_KERNINGS.at({lastCodePoint, codePoint});
            }
            chInfo = &FONT_CHARS_FIXED.at(codePoint);
            chData = &image_asset_data[0];

            if (chInfo && chData) {
                if (!calcWidthOnly) {
                    draw_char(x, y, *chInfo, chData);
                }
                x += chInfo->xadvance;
            }
            lastCodePoint = codePoint;
        }
        if (calcWidthOnly && chInfo) {
            x -= chInfo->xadvance;
            x += chInfo->width;
        }
        return x;

    }    

    void clear() {
        memset(framebuffer, 0, sizeof(framebuffer));
    }
       
    void write_frame() {
        send_cmd(CMD_SET_PAGE_ADRESS);
        send_cmd(CMD_SET_COLUMN_MSB);
        send_cmd(CMD_SET_COLUMN_LSB);
        for (uint16_t i = 0; i < sizeof(framebuffer); ++i) {
            send_dat(framebuffer[i]);
        }
    }

   private:
    uint8_t framebuffer[PAGES * COLUMNS] = {0xFF};
    bool initialized = false;

    void send_cmd(uint8_t v) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, LCD_CD_Pin, GPIO_PIN_RESET);
        HAL_SPI_Transmit(&hspi1, &v, 1, HAL_MAX_DELAY);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    }

    void send_dat(uint8_t v) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, LCD_CD_Pin, GPIO_PIN_SET);
        HAL_SPI_Transmit(&hspi1, &v, 1, HAL_MAX_DELAY);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    }

};

void screen_init() {
    ST7525Display::instance().init();
}

static int32_t seconds = 0;
static uint32_t adc_value[2] = {};

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *adc) {
    if ( adc == &hadc1 ) {
        static int32_t rank = 0;
        adc_value[rank] = HAL_ADC_GetValue(&hadc1);
        if (__HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_EOS)) {
            rank = 0;
        } else {
            rank ++;
            if ( rank >=2 ) {
                rank = 0;
            }
        }
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim == &htim1) {
        ST7525Display::instance().clear();
        static volatile int32_t c = 0;
        static char output[32] = {0};
        sprintf(output, "AIR:");
        ST7525Display::instance().draw_string(0, 0, output);
        sprintf(output, "%dpsi", int(c));
        ST7525Display::instance().draw_string(192/2-ST7525Display::instance().draw_string(0, 0, output, true)-8, 0, output);
        sprintf(output, "N2:");
        ST7525Display::instance().draw_string(192/2, 0, output);
        sprintf(output, "%dpsi", int(c));
        ST7525Display::instance().draw_string(192-ST7525Display::instance().draw_string(0, 0, output, true)-8, 0, output);

        int32_t h = (seconds  / 3600);
        int32_t m = (seconds  /   60) % 60;
        int32_t s = (seconds        ) % 60;
        sprintf(output, "%04d:%02d:%02d", int(h), int(m), int(s));
        ST7525Display::instance().draw_string(0, 44, output);

        sprintf(output, "%d", int(adc_value[0]));
        ST7525Display::instance().draw_string(0, 20, output);
        sprintf(output, "%d", int(adc_value[1]));
        ST7525Display::instance().draw_string(192/2, 20, output);

        c = c + 1;
        if (c > 120) c = 0;
        ST7525Display::instance().write_frame();

        HAL_ADC_Start_IT(&hadc1);
    }
    if (htim == &htim3) {
        seconds ++;

      //HAL_GPIO_WritePin(GPIOA, SOLENOID_VAVLE2_Pin, GPIO_PinState(c));
    }
}
