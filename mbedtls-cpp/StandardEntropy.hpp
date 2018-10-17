#pragma once

#include "Entropy.hpp"

#include <mbedtls/entropy.h>

namespace tls {
    
class StandardEntropy : public Entropy {
private:
    mbedtls_entropy_context mEntropy;
public:
    StandardEntropy()
    { 
        mbedtls_entropy_init(&mEntropy);
    }

    ~StandardEntropy() override 
    {
        mbedtls_entropy_free(&mEntropy);
    }
    
    EntropyCallback * getCallback() override 
    {
        return &mbedtls_entropy_func;
    }
    
    void * getData() override 
    {
        return &mEntropy;
    }
};

}
