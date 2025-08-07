/**
 * @file error.h
 * @brief Defines the I/O exception type for the config-cpp library.
 *
 * This file defines the `Config::IoError` exception class, which is used
 * for errors related to input/output operations. It inherits from
 * `Config::Error`.
 */
#pragma once

#include <config-cpp/error.h>

namespace Config {
/**
 * @brief An exception thrown for input/output errors.
 *
 * This class represents errors that occur during I/O operations.
 */
struct IoError : Error {
  using Error::Error;
};

}  // namespace Config
