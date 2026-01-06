#pragma once
#include <string>

enum class LogLevel { DEBUG, INFO, WARNING, ERROR };

enum class ErrorCode {
    SUCCESS,
    INVALID_INPUT,
    NOT_FOUND,
    FILE_ERROR,
    BUSINESS_RULE_VIOLATION
};
