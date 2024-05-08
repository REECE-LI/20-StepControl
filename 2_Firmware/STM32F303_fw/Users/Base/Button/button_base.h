/*
 * @Description: 稚晖的库 目前是基础功能测试 需要把 ButtonBase.Tick() 放入一个中断中 （源文件放在了一个100HZ的中断中）
 * @Author: X
 * @Date: 2023-03-26 10:10:01
 */
#ifndef BUTTON_BASE_H
#define BUTTON_BASE_H

#include <cstdint>

class ButtonBase {
public:
    enum Event {
        UP,
        DOWN,
        LONG_PRESS,
        CLICK
    };

    explicit ButtonBase(uint8_t _id):
        id(_id) {}

    ButtonBase(uint8_t _id, uint32_t _longPressTime):
        id(_id), longPressTime(_longPressTime) {}

    void Tick(uint32_t _timeElapseMillis);
    void SetOnEventListener(void (*_callback)(Event));

protected:
    uint8_t id;
    bool lastPinIO = true;
    uint32_t timer = 0;
    uint32_t pressTime{};
    uint32_t longPressTime = 2000;

    void (*OnEventFunc)(Event) {};

    virtual bool ReadButtonPinIO(uint8_t _id) = 0;
};

#endif
