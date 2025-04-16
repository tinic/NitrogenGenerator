#include "Core/Inc/main.h"

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

    double SystemTime() const {
        return system_time;
    }

    void SetRawPSI0(uint32_t v) {
        rank0 = v;
    }
    void SetRawPSI1(uint32_t v) {
        rank1 = v;
    }
    void SetSystemTime(double v) {
        system_time = v;
    }

    void Slice();

   private:
    void init() {
    }

    uint32_t rank0 = 0;
    uint32_t rank1 = 0;
    double system_time = 0;
    bool initialized = false;

    float adc_rank0_to_psi() const {
        return (float((rank0)-450.0f) / 1675.0f) * 100.0f;
    }

    float adc_rank1_to_psi() const {
        return (float((rank1)-450.0f) / 1675.0f) * 100.0f;
    }
};
