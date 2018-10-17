#pragma once

#include "Entropy.hpp"

namespace tls {

class RandomGenerator {
public:
    using generator_callback_t = int(void *, unsigned char *, size_t);

    virtual ~RandomGenerator() = default;
    virtual generator_callback_t * getCallback() = 0;
    virtual void * getContext() = 0;
    virtual void generate(unsigned char * destination, size_t size) = 0;
};

}
