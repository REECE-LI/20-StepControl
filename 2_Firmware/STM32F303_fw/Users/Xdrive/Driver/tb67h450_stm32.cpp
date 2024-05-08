#include "tb67h450_stm32.h"
#include "tim.h"
#include "dac.h"

void TB67H450::InitGpio() {
    // 程序初始化时已经初始化了GPIOA

}


void TB67H450::InitPwm() {
    // 初始化DAC 最开始已经初始化了 这里直接开启
    HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
    HAL_DAC_Start(&hdac, DAC_CHANNEL_2);

}


void TB67H450::DacOutputVoltage(uint16_t _voltageA_3300mVIn12bits, uint16_t _voltageB_3300mVIn12bits) {

    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, _voltageA_3300mVIn12bits);
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, _voltageB_3300mVIn12bits);
}


void TB67H450::SetInputA(bool _statusAp, bool _statusAm) {
    _statusAp ? (GPIOB->BSRR = GPIO_PIN_8) : (GPIOB->BRR = GPIO_PIN_8);
    _statusAm ? (GPIOB->BSRR = GPIO_PIN_9) : (GPIOB->BRR = GPIO_PIN_9);
}


void TB67H450::SetInputB(bool _statusBp, bool _statusBm) {
    _statusBp ? (GPIOB->BSRR = GPIO_PIN_6) : (GPIOB->BRR = GPIO_PIN_6);
    _statusBm ? (GPIOB->BSRR = GPIO_PIN_7) : (GPIOB->BRR = GPIO_PIN_7);
}
