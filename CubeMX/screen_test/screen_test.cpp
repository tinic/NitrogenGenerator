
#include "screen.h"
#include "mcp.h"

#include <iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "./Base64.h"
#include "./stb_image_write.h"

static void output() {
    auto bitmap = ST7525::instance().bitmap();
    auto bitmapSize = ST7525::instance().bitmapSize();
    const uint8_t *ptr = reinterpret_cast<const uint8_t *>(bitmap.data());
    static std::vector<uint8_t> png_data;
    png_data.clear();

    stbi_write_png_to_func([](void *context, void *data, int size){
        for (size_t c = 0; c < size; c++) {
            png_data.push_back(((uint8_t*)data)[c]);
        }
    }, nullptr, std::get<0>(bitmapSize), std::get<1>(bitmapSize), 4, ptr, std::get<0>(bitmapSize)*4);

    printf("\033]1337;File=inline=1;width=24:%s\a\n",macaron::Base64::Encode(png_data).c_str());
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

    MCP::instance().SetSystemTime(987*3600);
    MCP::instance().Slice();
    ST7525::instance().update();

    output();

    MCP::instance().SetRawPSI0(2400);
    MCP::instance().SetRawPSI1(2400);


    MCP::instance().SetSystemTime(654*3600+32*60+10);
    MCP::instance().Slice();

    MCP::instance().SetSolenoid0(true);
    MCP::instance().SetSolenoid1(true);

    ST7525::instance().update();

    output();
}
