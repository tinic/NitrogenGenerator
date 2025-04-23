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
#ifndef _MCP_H_
#define _MCP_H_

#include <cstdint>

#include "./Core/Inc/main.h"

class MCP {
   public:
    static MCP &instance();

    void SetSolenoid0(bool state) {
        HAL_GPIO_WritePin(GPIOA, SOLENOID_VALVE1_Pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
    };
    void SetSolenoid1(bool state) {
        HAL_GPIO_WritePin(GPIOA, SOLENOID_VAVLE2_Pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
    };
    bool Solenoid0() const {
        return (HAL_GPIO_ReadPin(GPIOA, SOLENOID_VALVE1_Pin) == GPIO_PIN_SET) ? true : false;
    }
    bool Solenoid1() const {
        return (HAL_GPIO_ReadPin(GPIOA, SOLENOID_VAVLE2_Pin) == GPIO_PIN_SET) ? true : false;
    }

    float PSI0() const {
        return adc_rank0_to_psi();
    }
    float PSI1() const {
        return adc_rank1_to_psi();
    }

    float SystemTime() const {
        return system_time;
    }
    float RefillElapsedTime() const {
        return refillElapsedTime;
    }

    void SetRawPSI0(uint32_t v) {
        rank0 = v;
    }
    void SetRawPSI1(uint32_t v) {
        rank1 = v;
    }
    void SetSystemTime(float v) {
        system_time = v;
    }
    void SetRefillElapsedTime(float v) {
        refillElapsedTime = v;
    }

    void SetFaultState(bool fault) {
        faultState = fault;
    }

    bool FaultState() const {
        return faultState;
    }

    void AddDutyCycleRecord(int32_t i, float v) {
        if (dutyCycleRecordFirstTime[i]) {
            dutyCycleRecordFirstTime[i] = false;
            for (size_t c = 0; c < dutyCycleRecordCount; c++) {
                dutyCycleRecord[i][c++] = v;
            }
            return;
        }
        dutyCycleRecord[i][dutyCycleRecordIndex++] = v;
        dutyCycleRecordIndex %= dutyCycleRecordCount;
    }

    float DutyCycleAverage(int32_t i) const {
        if ( i < 0 || i >= 2) {
            return 0.0f;
        }
        float dutyCycle = 0;
        for (size_t c = 0; c < dutyCycleRecordCount; c++) {
            dutyCycle += dutyCycleRecord[i][c];
        }
        return dutyCycle/float(dutyCycleRecordCount);
    }

    void Slice();

   private:
    void init() {
    }

    bool faultState = false;
    uint32_t rank0 = 0;
    uint32_t rank1 = 0;
    float refillElapsedTime = 0;
    float system_time = 0;
    bool initialized = false;

    static constexpr size_t dutyCycleRecordCount = 8;
    bool dutyCycleRecordFirstTime[2] = { true, true };
    float dutyCycleRecord[2][dutyCycleRecordCount] {};
    size_t dutyCycleRecordIndex = 0;

    float adc_rank0_to_psi() const {
        return ((float(rank0)-450.0f) / 1675.0f) * 100.0f;
    }

    float adc_rank1_to_psi() const {
        return ((float(rank1)-450.0f) / 1675.0f) * 100.0f;
    }
};

#endif  // #ifndef _MCP_H_
