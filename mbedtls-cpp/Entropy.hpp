#pragma once

#include <cstdlib>

namespace tls {

class Entropy {
public:
    using EntropyCallback = int (void *, unsigned char *, size_t);

    virtual ~Entropy() = default;
    
    virtual EntropyCallback * getCallback() = 0;
    virtual void * getData() = 0;
};

}
