#include "Core/Inc/main.h"

#include <cstdio>
#include <cstring>

#include "screen.h"
#include "mcp.h"

extern "C" ADC_HandleTypeDef hadc1;
extern "C" UART_HandleTypeDef huart1;
extern "C" TIM_HandleTypeDef htim1;
extern "C" TIM_HandleTypeDef htim3;

static double system_time = 0;

static uint32_t adc_value[32] = {};

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *adc) {
    if ( adc == &hadc1 ) {
        static int32_t rank = 0;
        adc_value[rank] = HAL_ADC_GetValue(&hadc1);
        if (__HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_EOS)) {
            rank = 0;
        } else {
            rank ++;
            rank %= 32;
        }
        MCP::instance().SetRawPSI0(adc_value[0]);
        MCP::instance().SetRawPSI1(adc_value[1]);
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim == &htim1) {
        system_time += 1.0/100.0;
        MCP::instance().SetSystemTime(system_time);

        screen_clear();
        static volatile int32_t c = 0;
        static char output[32] = {0};
        sprintf(output, "AIR:");
        screen_draw_string(0, 0, output);
        sprintf(output, "%dpsi", int(MCP::instance().PSI0()));
        screen_draw_string(192/2-screen_draw_string(0, 0, output, true)-8, 0, output);
        sprintf(output, "N2:");
        screen_draw_string(192/2, 0, output);
        sprintf(output, "%dpsi", int(MCP::instance().PSI1()));
        screen_draw_string(192-screen_draw_string(0, 0, output, true)-8, 0, output);

        int32_t h = (int(system_time)  / 3600);
        int32_t m = (int(system_time)  /   60) % 60;
        int32_t s = (int(system_time)        ) % 60;
        sprintf(output, "%04d:%02d:%02d", int(h), int(m), int(s));
        screen_draw_string(0, 44, output);

        c = c + 1;
        if (c > 120) c = 0;
        screen_flip();

        HAL_ADC_Start_IT(&hadc1);
    }
    if (htim == &htim3) {
        MCP::instance().Slice();
    }
}
