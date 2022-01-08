#include "Exception.hpp"

#include <cstring>

#include <mbedtls/error.h>

#include <esp_log.h>

namespace tls {
std::string errorString(int error_code) {
    constexpr int size = 128;
    char error_buffer[size];
    mbedtls_strerror(error_code, error_buffer, size);
    return std::string(error_buffer);
}

Exception::Exception(int code) :
    std::runtime_error(errorString(code)),
    mErrorCode(code)
{
}

int Exception::error_code() const noexcept {
    return mErrorCode;
}
}
