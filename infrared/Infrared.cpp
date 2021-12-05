#include "Infrared.hpp"

#include <luna/EventLoop.hpp>

#include <driver/gpio.h>

#include <bitset>
#include <cstring>

namespace luna {
    namespace
    {
        constexpr auto clock = 80000000.0;
        constexpr auto divider = 160;
        constexpr auto resolutionUs = divider / clock * 1000000.0;

        constexpr auto usToTicks(double interval) {
            return static_cast<uint32_t>(interval / resolutionUs);
        }

        constexpr auto unitdaw = 562.5;
        constexpr auto unit = 562.5;
        constexpr auto pulseLength = usToTicks(unit);
        constexpr auto lowPulseLength = usToTicks(unit);
        constexpr auto highPulseLength = usToTicks(unit * 3);
        constexpr auto leadingHigh = usToTicks(unit * 16);
        constexpr auto leadingLow = usToTicks(unit * 8);

        struct NecBit {
            bool valid = false;
            bool value = false;
        };

        struct NecMessage {
            bool received = false;
            uint8_t address = 0;
            uint8_t command = 0;
        };

        template<uint32_t target>
        bool isNear(uint32_t value)
        {
            return (target * 8 / 10) < value && value < (target * 12 / 10);
        }

        bool isPulseLeading(rmt_item32_t const & item)
        {
            return item.level0 == 0 && isNear<leadingHigh>(item.duration0)
                && item.level1 == 1 && isNear<leadingLow>(item.duration1);
        }

        NecBit decodeBit(rmt_item32_t const & item) {
            NecBit bit;
            if (item.level0 == 0 && isNear<pulseLength>(item.duration0)
                && item.level1 == 1
                && item.duration1 > (lowPulseLength * 8 / 10)
                && item.duration1 < (highPulseLength * 12 / 10))
            {
                bit.valid = true;
                bit.value = item.duration1 > (lowPulseLength + highPulseLength) / 2;
            }
            return bit;
        }


        NecMessage decodeMessage(rmt_item32_t * pulses, size_t size)
        {
            NecMessage message;
            if (size >= 33 && isPulseLeading(pulses[0])) {
                std::bitset<32> payload;
                for (int i = 0; i < 32; ++i) {
                    auto bit = decodeBit(pulses[i + 1]);
                    if (!bit.valid) {

                        return message;
                    }
                    payload[i] = bit.value;
                }

                union {
                    uint32_t integer;
                    struct {
                        uint8_t address;
                        uint8_t addressInverted;
                        uint8_t command;
                        uint8_t commandInverted;
                    };
                } raw;

                raw.integer = payload.to_ulong();
                if ((raw.address ^ raw.addressInverted) == 0xFF && (raw.command ^ raw.commandInverted) == 0xFF) {
                    message.received = true;
                    message.address = raw.address;
                    message.command = raw.command;
                }
            }

            return message;
        }
    }

    Infrared::Infrared(EventLoop * mainLoop, int pin, InfraredDemux * demultiplexer) :
        mMainLoop(mainLoop),
        mDemultiplexer(demultiplexer)
    {
        {
            gpio_config_t config{
                .pin_bit_mask = 1ull << pin,
                .mode = GPIO_MODE_INPUT,
                .pull_up_en = (gpio_pullup_t) 0,
                .pull_down_en = (gpio_pulldown_t) 0,
                .intr_type = (gpio_int_type_t)GPIO_PIN_INTR_DISABLE,
            };
            gpio_config(&config);
        }
        
        // TODO: integrate with WS281x driver and make channel assignment automatic
        auto const channel = static_cast<rmt_channel_t>(7);

        rmt_config_t config = {
            .rmt_mode = RMT_MODE_RX,
            .channel = channel,
            .gpio_num = static_cast<gpio_num_t>(pin),
            .clk_div = divider,
            .mem_block_num = 1,
            .rx_config = {
                .idle_threshold = leadingHigh * 11 / 10,
            },
        };

        rmt_config(&config);
        rmt_driver_install(channel, 300, 0);

        rmt_get_ringbuf_handle(channel, &mBuffer);
        rmt_rx_start(channel, true);

        xTaskCreate([](void * data) {
            static_cast<Infrared *>(data)->receive();
            vTaskDelete(nullptr);
        }, "IR", 2 * 1024, this, 5, &mTaskHandle);
    }

    void Infrared::receive()
    {
        for (;;) {
            size_t length;
            auto items = reinterpret_cast<rmt_item32_t *>(xRingbufferReceive(mBuffer, &length, 300));
            if (items) {
                length /= 4;
                auto message = decodeMessage(items, length);
                if (message.received) {
                    ESP_LOGI("IR", "Address 0x%x, command 0x%x", message.address, message.command);
                    mMainLoop->post([this, message]{
                        mDemultiplexer->demultiplex(message.address, message.command);
                    });
                }
                vRingbufferReturnItem(mBuffer, (void *) items);
            }
        }
    }
}
