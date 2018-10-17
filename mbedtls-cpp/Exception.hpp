#pragma once

#include <stdexcept>
#include <memory>


namespace tls {

namespace detail {
    std::shared_ptr<char> errorSharedString(int error_code);
}

std::string errorString(int error_code);

class Exception : public std::exception {
private:
    int mErrorCode;
    std::shared_ptr<char> mMessage;
public:
    Exception(int code);

    const char * what() const noexcept override;
    int error_code() const noexcept;
};

}
