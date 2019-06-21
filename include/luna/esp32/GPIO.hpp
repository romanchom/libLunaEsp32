#pragma once

#include <driver/gpio.h>

namespace luna::esp32
{
    
struct GPIO
{
    explicit GPIO(int pin) :
        mPin(gpio_num_t(pin))
    {
        gpio_config_t io_conf;
        io_conf.intr_type = gpio_int_type_t(GPIO_PIN_INTR_DISABLE);
        io_conf.mode = GPIO_MODE_OUTPUT;
        io_conf.pin_bit_mask = 1 << 14;
        io_conf.pull_down_en = gpio_pulldown_t(0);
        io_conf.pull_up_en = gpio_pullup_t(0);
        gpio_config(&io_conf);

        out(false);
    }

    void out(bool level)
    {
        gpio_set_level(mPin, level ? 1 : 0);
    }
private:
    gpio_num_t mPin;
};

}
