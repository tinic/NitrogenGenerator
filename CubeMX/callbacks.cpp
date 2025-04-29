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

static int32_t adc_channel = 0;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *adc) {
    if ( adc == &hadc1 ) {
        if (__HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_EOS)) {
            uint32_t adc_value = HAL_ADC_GetValue(&hadc1);
            if (adc_channel == 1) {
                MCP::instance().SetRawPSI1(adc_value);
            } else {
                MCP::instance().SetRawPSI0(adc_value);
            }
        }
    }
}

extern "C" TIM_HandleTypeDef htim1;
extern "C" TIM_HandleTypeDef htim3;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim == &htim1) {
        // 1/10s interval
        static float system_time = 0;
        system_time += 1.0f/10.0f;
        MCP::instance().SetSystemTime(system_time);
        ST7525::instance().update();

        // Using ranked sampling seems to be broken on this MCU, so do one channel at a time.
        ADC_ChannelConfTypeDef sConfig {};
        adc_channel ^= 1;
        sConfig.Channel = adc_channel ? ADC_CHANNEL_4 : ADC_CHANNEL_3;
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
        HAL_ADC_ConfigChannel(&hadc1, &sConfig);
        HAL_ADC_Start_IT(&hadc1);
    }
    if (htim == &htim3) {
        // 1s interval
        MCP::instance().Slice();
    }
}
