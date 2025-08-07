#pragma once
/**
 * @file error.h
 * @brief Defines the exception type for JSON-related errors in the config-cpp
 * library.
 *
 * This file introduces the `Config::JsonError` exception class, which is thrown
 * when errors occur during JSON parsing or manipulation. It inherits from the
 * base exception class `Config::Error`.
 */

#include <config-cpp/error.h>

namespace Config {

/**
 * @brief Exception for JSON-related errors.
 *
 * This exception is thrown when an error occurs during JSON parsing or
 * manipulation.
 */
struct JsonError : Error {
  using Error::Error;
};

};  // namespace Config
