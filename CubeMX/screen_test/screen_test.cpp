
#include "screen.h"
#include "mcp.h"

#include <iostream>

#include "../sixel-tools/sixel.h"

static void output() {
    sixel::image<sixel::format_1bit, ST7525::COLUMNS*2, ST7525::LINES*2> image;
    image.copy(ST7525::instance().bitmap1Bit());
    image.sixel([](uint8_t ch){
        putc(ch,stdout);
    });
    printf("\n");
}

constexpr std::vector<uint8_t> test() {
    std::vector<uint8_t> out {};
    sixel::image<sixel::format_1bit, ST7525::COLUMNS*2, ST7525::LINES*2> image;
    image.clear();
    image.fillrect(0,0,20,20,1);
    image.sixel([&out](uint8_t ch) mutable {
        out.push_back(ch);
    });
    return out;
}

int main() {

    static_assert(test().size() == 420);

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
