#include "screen.h"
#include "mcp.h"

#include <fixed_containers/fixed_map.hpp>

#include "font_0.h"
#include "Core/Inc/main.h"

extern "C" SPI_HandleTypeDef hspi1;

ST7525 &ST7525::instance() {
    static ST7525 st7525display;
    if (!st7525display.initialized) {
        st7525display.initialized = true;
        st7525display.init();
    }
    return st7525display;
}

void ST7525::init() {
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

void ST7525::set_pixel(int32_t x, int32_t y, uint8_t c) {
    if (x >= COLUMNS || y >= LINES)
        return;
    uint16_t idx = uint16_t((y / 8) * COLUMNS + x);
    if (c)
        framebuffer[idx] |= uint8_t( (1 << (y % 8)));
    else
        framebuffer[idx] &= uint8_t(~(1 << (y % 8)));
}

void ST7525::draw_char(int32_t x, int32_t y, const CharInfo &ch, const uint8_t *data) {
    const int32_t stride = (IMAGE_ASSET_WIDTH + 7) & ~(7);
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

int32_t ST7525::draw_string(int32_t x, int32_t y, const char *str, bool calcWidthOnly) {
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

        if (fontKerningCount) {
            if (fontKerningMap.contains({lastCodePoint, codePoint})) {
                x += fontKerningMap.at({lastCodePoint, codePoint});
            }
        }

        chInfo = 0;
        chData = 0;
        if (charInfoMap.contains(uint8_t(codePoint))) {
            chInfo = &charInfoMap.at(uint8_t(codePoint));
            chData = &image_asset_data[0];
        }

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

void ST7525::clear() {
    memset(framebuffer, 0, sizeof(framebuffer));
}
    
void ST7525::write_frame() {
    send_cmd(CMD_SET_PAGE_ADRESS);
    send_cmd(CMD_SET_COLUMN_MSB);
    send_cmd(CMD_SET_COLUMN_LSB);
    for (uint16_t i = 0; i < sizeof(framebuffer); ++i) {
        send_dat(framebuffer[i]);
    }
}

void ST7525::update() {
    clear();

    static char output[32] = {0};
    sprintf(output, "AIR:");
    draw_string(0, 0, output);
    sprintf(output, "%dpsi", int(MCP::instance().PSI0()));
    draw_string(192/2-draw_string(0, 0, output, true)-4, 0, output);
    sprintf(output, "N2:");
    draw_string(192/2+4, 0, output);
    sprintf(output, "%dpsi", int(MCP::instance().PSI1()));
    draw_string(191-draw_string(0, 0, output, true), 0, output);

    sprintf(output, "%s", MCP::instance().Solenoid0() ? "ON" : "OFF");
    draw_string(0, 22, output);
    sprintf(output, "%s", MCP::instance().Solenoid1() ? "ON" : "OFF");
    draw_string(192/2+4, 22, output);

    sprintf(output, "%02d%%", 12);// int(MCP::instance().DutyCycleAverage()*100.0f));
    draw_string(191-draw_string(0, 0, output, true), 22, output);

    int32_t h = (int(MCP::instance().SystemTime())  / 3600);
    int32_t m = (int(MCP::instance().SystemTime())  /   60) % 60;
    int32_t s = (int(MCP::instance().SystemTime())        ) % 60;
    sprintf(output, "T:%04d:%02d:%02d", int(h), int(m), int(s));
    draw_string(0, 44, output);

    int32_t em = (int(MCP::instance().RefillElapsedTime())  /   60) % 60;
    int32_t es = (int(MCP::instance().RefillElapsedTime())        ) % 60;
    sprintf(output, "R:%02d:%02d", int(em), int(es));
    draw_string(191-draw_string(0, 0, output, true), 44, output);

    write_frame();
}

void ST7525::send_cmd(uint8_t v) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, LCD_CD_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &v, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
}

void ST7525::send_dat(uint8_t v) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, LCD_CD_Pin, GPIO_PIN_SET);
    HAL_SPI_Transmit(&hspi1, &v, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
}

void screen_init() {
    ST7525::instance().init();
}
