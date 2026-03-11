#pragma once

#include "error_code.hpp"
#include <stdexcept>
#include <string>

namespace a2a {

/**
 * @brief Exception class for A2A protocol errors
 */
class A2AException : public std::runtime_error {
public:
    A2AException(const std::string& message, ErrorCode code)
        : std::runtime_error(message) 
        , error_code_(code) 
        , request_id_() {}

    /**
     * @brief Get error code as integer
     */
    int32_t error_code_value() const noexcept {
        return static_cast<int32_t>(error_code_);
    }

private:
    ErrorCode error_code_;
    std::string request_id_;
};
}   // namespace a2a