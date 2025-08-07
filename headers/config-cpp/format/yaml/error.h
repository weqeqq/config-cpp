/**
 * @file error.h
 * @brief Defines the exception type for YAML-related errors.
 *
 * This file introduces the `Config::YamlError` exception class, which is used
 * for all exceptions thrown by the library during YAML processing. It inherits
 * from the base `Config::Error` exception class.
 */
#pragma once

#include <config-cpp/error.h>

namespace Config {

/**
 * @brief An exception class for YAML-related errors.
 *
 * This exception is thrown for errors encountered during the processing of YAML
 * data. It inherits from `Config::Error`.
 */
struct YamlError : Error {
  using Error::Error;
};

};  // namespace Config
