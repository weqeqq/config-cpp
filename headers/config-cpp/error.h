/**
 * @file error.h
 * @brief Defines the base exception type for the config-cpp library.
 *
 * This file introduces the `Config` namespace and defines the `Config::Error`
 * exception class, which serves as the base for all exceptions thrown by the
 * library. `Config::Error` inherits from `std::runtime_error`.
 */
#pragma once

#include <stdexcept>
#include <string_view>

namespace Config {
/**
 * @brief The base exception class for the config-cpp library.
 *
 * All exceptions thrown by the library will derive from this class.
 */
struct Error : std::runtime_error {
  /**
   * @brief Constructs an Error exception.
   * @param message The error message associated with the exception.
   */
  Error(std::string_view message) : std::runtime_error(message.data()) {}
};
};  // namespace Config
