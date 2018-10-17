#include "Exception.hpp"

#include <cstring>

#include <mbedtls/error.h>

namespace tls {

namespace detail {
    std::shared_ptr<char> errorSharedString(int error_code) {
        constexpr int size = 128;
        char error_buffer[size];
        mbedtls_strerror(error_code, error_buffer, size);
        int length = strlen(error_buffer);

        std::shared_ptr<char> ret(new char[length + 1], std::default_delete<char[]>());
        memcpy(ret.get(), error_buffer, length + 1);
        return ret;
    }
}

std::string errorString(int error_code) {
    constexpr int size = 128;
    char error_buffer[size];
    mbedtls_strerror(error_code, error_buffer, size);
    return std::string(error_buffer);
}

Exception::Exception(int code) :
    mErrorCode(code),
    mMessage(detail::errorSharedString(code))
{}

const char * Exception::what() const noexcept {
    return mMessage.get();
}

int Exception::error_code() const noexcept {
    return mErrorCode;
}

}
