#pragma once

#include <stdexcept>
#include <memory>


namespace tls {
class Exception : public std::runtime_error {
private:
    int mErrorCode;
public:
    Exception(int code);
    int error_code() const noexcept;
};

}
