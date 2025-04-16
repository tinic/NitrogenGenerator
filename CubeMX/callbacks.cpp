#include "Core/Inc/main.h"

#include <cstdio>
#include <cstring>

#include "screen.h"

extern "C" ADC_HandleTypeDef hadc1;
extern "C" UART_HandleTypeDef huart1;
extern "C" TIM_HandleTypeDef htim1;
extern "C" TIM_HandleTypeDef htim3;

static int32_t seconds = 0;

static uint32_t adc_value[32] = {};

static float adc_rank0_to_psi() {
    return (float((adc_value[0])-450.0f)/1675.0f)*100.0f;
}

static float adc_rank1_to_psi() {
    return (float((adc_value[1])-450.0f)/1675.0f)*100.0f;
}

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
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim == &htim1) {
        screen_clear();
        static volatile int32_t c = 0;
        static char output[32] = {0};
        sprintf(output, "AIR:");
        screen_draw_string(0, 0, output);
        sprintf(output, "%dpsi", int(adc_rank0_to_psi()));
        screen_draw_string(192/2-screen_draw_string(0, 0, output, true)-8, 0, output);
        sprintf(output, "N2:");
        screen_draw_string(192/2, 0, output);
        sprintf(output, "%dpsi", int(adc_rank1_to_psi()));
        screen_draw_string(192-screen_draw_string(0, 0, output, true)-8, 0, output);

        int32_t h = (seconds  / 3600);
        int32_t m = (seconds  /   60) % 60;
        int32_t s = (seconds        ) % 60;
        sprintf(output, "%04d:%02d:%02d", int(h), int(m), int(s));
        screen_draw_string(0, 44, output);

        c = c + 1;
        if (c > 120) c = 0;
        screen_flip();

        HAL_ADC_Start_IT(&hadc1);
    }
    if (htim == &htim3) {
        seconds ++;

        HAL_GPIO_WritePin(GPIOA, SOLENOID_VALVE1_Pin, GPIO_PIN_SET);
    }
}
