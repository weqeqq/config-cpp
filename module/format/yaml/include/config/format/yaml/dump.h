/**
 * @file 
 * @brief YAML format utilities for configuration serialization.
 *
 * This header provides functionality for serializing configuration nodes to YAML format.
 * It includes a custom error type for YAML dumping and a function to convert a configuration
 * node to its YAML string representation.
 */

#pragma once

#include <config/format/yaml/error.h>
#include <config/format/yaml/export.h>
#include <config/core/node.h>

/**
 * @namespace Config::YamlFormat
 * @brief Namespace for YAML format-related functionality.
 *
 * Contains utilities for handling YAML serialization of configuration nodes, including
 * error handling and export definitions.
 */
namespace Config::YamlFormat {

/**
 * @struct DumpError
 * @brief Exception class for errors occurring during YAML serialization.
 *
 * Inherits from YamlError to provide specific error handling for YAML dump operations.
 */
struct DumpError : YamlError {
  /**
   * @brief Constructs a DumpError with a specific error message.
   * @param message A string view containing the error message.
   */
  DumpError(std::string_view message)
    : YamlError(Message("DumpError", message)) {}
};

/**
 * @brief Serializes a configuration node to a YAML string.
 * @param node The configuration node to serialize.
 * @return A string containing the YAML representation of the node.
 * @throw DumpError if serialization fails.
 */
CONFIG_FORMAT_YAML_EXPORT;
std::string Dump(const Node &node);

}
