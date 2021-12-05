#pragma once

#include <stdexcept>

#include <mbedtls/ctr_drbg.h>

#include "RandomGenerator.hpp"
#include "Exception.hpp"

namespace tls {
    class CounterDeterministicRandomGenerator : public RandomGenerator {
    private:
        mbedtls_ctr_drbg_context m_generator;
    public:
        explicit CounterDeterministicRandomGenerator()
        {
            mbedtls_ctr_drbg_init(&m_generator);
        }

        explicit CounterDeterministicRandomGenerator(Entropy * entropy_source, const char * data, size_t dataLength) :
            CounterDeterministicRandomGenerator()
        {
            seed(entropy_source, data, dataLength);
        }

        ~CounterDeterministicRandomGenerator() override
        {
            mbedtls_ctr_drbg_free(&m_generator);
        }

        virtual generator_callback_t * getCallback() override
        {
            return &mbedtls_ctr_drbg_random;
        };

        virtual void * getContext() override
        {
            return reinterpret_cast<void *>(&m_generator);
        }

        void seed(Entropy * entropy_source, const char * data, size_t dataLength)
        {
            auto error = mbedtls_ctr_drbg_seed(&m_generator,
                entropy_source->getCallback(), entropy_source->getData(),
                reinterpret_cast<const unsigned char *>(data), dataLength);
            if (0 != error) {
                std::terminate();
                // throw tls::Exception(error);
            }
        }

        void generate(unsigned char * destination, size_t size) override
        {
            int error = mbedtls_ctr_drbg_random(reinterpret_cast<void *>(&m_generator), destination, size);
            if (0 != error) {
                std::terminate();
                // throw tls::Exception(error);
            }
        }
    };
}