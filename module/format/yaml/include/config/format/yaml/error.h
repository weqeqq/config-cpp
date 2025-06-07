/**
 * @file 
 * @brief Defines the YamlError class for handling YAML-related errors in the Config namespace.
 */
#pragma once 

#include <config/core/error.h>

namespace Config {

/**
 * @brief A class representing errors specific to YAML processing.
 *
 * This class inherits from the base Error class and is used to encapsulate
 * errors that occur during YAML configuration handling. It provides a way
 * to store and retrieve error messages specific to YAML operations.
 */
class YamlError : public Error {
 public: 
  /**
   * @brief Constructs a YamlError with a specific error message.
   *
   * The constructor initializes the base Error class with a namespace-qualified
   * error message, prefixing it with "YamlError".
   *
   * @param message A string view containing the error message.
   */
  YamlError(std::string_view message) 
    : Error(Namespace("YamlError", message)) {}
};
};
