#include "Core/Inc/main.h"

#include <cstdio>
#include <cstring>

#include "screen.h"
#include "mcp.h"

extern "C" ADC_HandleTypeDef hadc1;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *adc) {
    if ( adc == &hadc1 ) {
        static uint32_t adc_value[8] = {};
        static int32_t rank = 0;
        adc_value[rank] = HAL_ADC_GetValue(&hadc1);
        if (__HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_EOS)) {
            rank = 0;
        } else {
            rank ++;
            rank %= 8;
        }
        MCP::instance().SetRawPSI0(adc_value[0]);
        MCP::instance().SetRawPSI1(adc_value[1]);
    }
}

extern "C" TIM_HandleTypeDef htim1;
extern "C" TIM_HandleTypeDef htim3;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim == &htim1) {
        static double system_time = 0;
        system_time += 1.0/100.0;
        MCP::instance().SetSystemTime(system_time);
        screen_update();
        HAL_ADC_Start_IT(&hadc1);
    }
    if (htim == &htim3) {
        MCP::instance().Slice();
    }
}
