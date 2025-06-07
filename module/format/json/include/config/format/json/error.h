/**
 * @file
 * @brief Defines the JsonError class for handling JSON-related errors.
 *
 * This file contains the declaration of the JsonError class, which extends the base Error class
 * to provide specific error handling for JSON operations within the Config namespace.
 */
#pragma once

#include <config/core/error.h>

namespace Config {

/**
 * @class JsonError
 * @brief A class representing errors specific to JSON processing.
 *
 * The JsonError class is derived from the base Error class and is used to encapsulate
 * errors that occur during JSON-related operations. It provides a constructor to initialize
 * the error with a specific message.
 */
class JsonError : public Error {
 public:
  /**
   * @brief Constructs a JsonError with a specified error message.
   *
   * @param message A string view containing the error message.
   */
  JsonError(
    std::string_view message
  ) : Error(Namespace("JsonError", message)) {}
};
};

