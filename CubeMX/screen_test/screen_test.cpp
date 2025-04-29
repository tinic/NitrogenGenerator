
#include "screen.h"
#include "mcp.h"

#include <iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "./Base64.h"
#include "./stb_image_write.h"

std::vector<uint8_t> png_data;

static void write_func(void *context, void *data, int size) {
    for (size_t c = 0; c < size; c++) {
        png_data.push_back(((uint8_t*)data)[c]);
    }
}

static void output() {
    png_data.clear();
    auto bitmap = ST7525::instance().bitmap();
    const uint8_t *ptr = reinterpret_cast<const uint8_t *>(bitmap.data());
    stbi_write_png_to_func(write_func, nullptr, ST7525::COLUMNS, ST7525::LINES, 4, ptr, ST7525::COLUMNS*4);
    auto base64 = macaron::Base64::Encode(png_data);
    printf("\033]1337;File=inline=1;width=24:%s\a\n",base64.c_str());
}

int main() {
    MCP::instance().SetSystemTime(0);
    MCP::instance().Slice();
    ST7525::instance().update();

    output();

    MCP::instance().SetSystemTime(5);
    MCP::instance().Slice();
    ST7525::instance().update();

    output();

    MCP::instance().SetSystemTime(10);
    MCP::instance().Slice();
    ST7525::instance().update();

    output();

    MCP::instance().SetRawPSI0(500);
    MCP::instance().SetRawPSI1(500);

    MCP::instance().SetSystemTime(10);
    MCP::instance().Slice();
    ST7525::instance().update();

    output();
}
