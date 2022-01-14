#pragma once

#include "mbed.h"

class LightEffect
{
public:
    LightEffect(PinName pin) : m_led(pin)
    {
        m_led.period_ms(1);
        m_led.write(1.0f);
    };
    ~LightEffect() = default;

    void TimerCallback()
    {
        m_duty += m_step;
        if(m_duty <= 0.5f || m_duty >= 1.0f)
            m_step = m_step*-1;
        m_led.write(m_duty);
        m_timer.attach(callback(this, &LightEffect::TimerCallback), std::chrono::milliseconds(10));
    };

    void On()
    {
        m_duty = 1.0f;
        m_led.write(m_duty);
        m_timer.attach(callback(this, &LightEffect::TimerCallback), std::chrono::milliseconds((int)(1000/m_flashFreq)));
    };

    void Off()
    {
        m_timer.detach();
        m_led.write(m_duty);
    }

    void Period(float period)
    {
        m_period = period;
        m_step = 1.0f/m_period/m_flashFreq * -1;
        m_duty = 1.0f;
    }

protected:
    PwmOut m_led;
    float m_duty = 1.0f;
    float m_period = 1.0f;
    float m_step = -0.01f;
    int m_flashFreq = 100;
    Timeout m_timer;
};

class FlashLED : DigitalOut
{
public:
    FlashLED(PinName pin, int on = 0) : DigitalOut(pin)
    {
        write(!on);
    }
    ~FlashLED() = default;

    void Flash(int ms = 50)
    {
        if(m_ledStatus == 0)
        {
            m_ledStatus = 2;
            (*this) = 1;
            m_flashTimer.attach(callback(this, &FlashLED::TimerCallback), std::chrono::milliseconds(ms));
        }
    }

    void TimerCallback()
    {
        if(m_ledStatus == 2)
        {
            m_flashTimer.attach(callback(this, &FlashLED::TimerCallback), std::chrono::milliseconds(50));
            m_ledStatus = 1;
            (*this) = 0;
        }
        else
        {
            m_ledStatus = 0;
        }
    }

    FlashLED &operator= (int value)
    {
        write(!value);
        return *this;
    }

    operator int()
    {
        return !read();
    }

private:
    Timeout m_flashTimer;
    int m_ledStatus;
};
