
#include "screen.h"
#include "mcp.h"

#include <iostream>


int main() {
    MCP::instance().SetSystemTime(0);
    MCP::instance().Slice();
    ST7525::instance().update();

    ST7525::instance().output();

    MCP::instance().SetSystemTime(5);
    MCP::instance().Slice();
    ST7525::instance().update();

    ST7525::instance().output();

    MCP::instance().SetSystemTime(10);
    MCP::instance().Slice();
    ST7525::instance().update();

    ST7525::instance().output();

    MCP::instance().SetRawPSI0(500);
    MCP::instance().SetRawPSI1(500);

    MCP::instance().SetSystemTime(987*3600);
    MCP::instance().Slice();
    ST7525::instance().update();

    ST7525::instance().output();

    MCP::instance().SetRawPSI0(2400);
    MCP::instance().SetRawPSI1(2400);


    MCP::instance().SetSystemTime(654*3600+32*60+10);
    MCP::instance().Slice();

    MCP::instance().SetSolenoid0(true);
    MCP::instance().SetSolenoid1(true);

    ST7525::instance().update();

    ST7525::instance().output();
}
