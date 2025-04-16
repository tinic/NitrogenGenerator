#include "mcp.h"

#include "Core/Inc/main.h"

MCP &MCP::instance() {
    static MCP mcp;
    if (!mcp.initialized) {
        mcp.initialized = true;
        mcp.init();
    }
    return mcp;
}

void MCP::Slice() {
    static int32_t timeElapsedRefill = 0;

    static int32_t timerRefill = 0;
    static bool timerRefillActive = false;

    static int32_t timerShutoff = 0;
    static bool timerShutoffActive = false;

    bool solenoid0 = false;
    bool solenoid1 = false;

    if (PSI1() < 75.0f) {
        timerRefillActive = true;
    }

    if (PSI1() >= 100.0f) {
        timerShutoffActive = true;
    }

    if (timerRefill >= 10) {
        solenoid0 = true;
        if (PSI0() > 75.0f) {
            solenoid1 = true;
            SetRefillElapsedTime(double(timeElapsedRefill++));
        }
    }

    if (timerShutoff >= 5) {
        solenoid0 = false;
        solenoid1 = false;
        timerRefillActive = false;
        timerRefill = 0;
        timerShutoffActive = false;
        timerShutoff = 0;
        timeElapsedRefill = 0;
    }

    if (timerRefillActive) {
        timerRefill++;
    }

    if (timerShutoffActive) {
        timerShutoff++;
    }

    SetSolenoid0(solenoid0);
    SetSolenoid1(solenoid1);
}
