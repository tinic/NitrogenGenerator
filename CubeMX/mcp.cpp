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
#include "./mcp.h"

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

    if (PSI0() >= 130.0f || PSI1() >= 130.0f) {
        SetSolenoid0(false);
        SetSolenoid1(false);
        return;
    }

    bool resetDuty = false;

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
            SetRefillElapsedTime(static_cast<float>(timeElapsedRefill++));
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
        resetDuty = true;
    }

    if (timerRefillActive) {
        timerRefill++;
    }

    if (timerShutoffActive) {
        timerShutoff++;
    }

    static int32_t dutyTotal = 0;
    static int32_t dutyRefill[2] = { 0, 0 };
    dutyTotal++;
    if (solenoid0) {
        dutyRefill[0]++;
    }
    if (solenoid1) {
        dutyRefill[1]++;
    }
    if (resetDuty) {
        AddDutyCycleRecord(0, static_cast<float>(dutyRefill[0]) / static_cast<float>(dutyTotal));
        AddDutyCycleRecord(1, static_cast<float>(dutyRefill[1]) / static_cast<float>(dutyTotal));
        
        dutyTotal = 0;
        dutyRefill[0] = 0;
        dutyRefill[1] = 0;
    }

    SetSolenoid0(solenoid0);
    SetSolenoid1(solenoid1);
}
