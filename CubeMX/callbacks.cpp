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
#include <cstdio>
#include <cstring>

#include "./Core/Inc/main.h"

#include "./screen.h"
#include "./mcp.h"

extern "C" ADC_HandleTypeDef hadc1;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *adc) {
#if 1
    if ( adc == &hadc1 ) {
        static uint32_t adc_value[8] = {};
        static int32_t rank = 0;
        adc_value[rank] = HAL_ADC_GetValue(&hadc1);
        if (__HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_EOS)) {
            rank = 0;
        } else {
            rank++;
            rank %= 8;
        }
        MCP::instance().SetRawPSI0(adc_value[0]);
        MCP::instance().SetRawPSI1(adc_value[1]);
    }
#endif  // #if 0
}

extern "C" TIM_HandleTypeDef htim1;
extern "C" TIM_HandleTypeDef htim3;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
#if 1
    if (htim == &htim1) {
        // 1/10s interval
        static float system_time = 0;
        system_time += 1.0f/10.0f;
        MCP::instance().SetSystemTime(system_time);
        ST7525::instance().update();
        HAL_ADC_Start_IT(&hadc1);
    }
    if (htim == &htim3) {
        // 1s interval
        MCP::instance().Slice();
    }
#endif  // #if 0
}
