/**
 * @file 
 * @brief YAML format parsing functionality for configuration management.
 *
 * This header defines the YAML parsing functionality within the Config namespace,
 * including error handling and node parsing for YAML content.
 */

#pragma once

#include <config/format/yaml/export.h>
#include <config/format/yaml/error.h>
#include <config/core/node.h>

/**
 * @namespace Config::YamlFormat
 * @brief Namespace for YAML format parsing and related utilities.
 *
 * Contains classes and functions for parsing YAML content into configuration nodes
 * and handling associated errors.
 */
namespace Config::YamlFormat {

/**
 * @class ParseError
 * @brief Exception class for YAML parsing errors.
 *
 * Inherits from YamlError to provide specific error handling for issues encountered
 * during YAML parsing.
 */
struct ParseError : YamlError {
  /**
   * @brief Constructs a ParseError with a specific error message.
   *
   * @param message A string view containing the error message.
   */
  ParseError(std::string_view message)
    : YamlError(Message("ParseError", message)) {}
};

/**
 * @brief Parses YAML content into a configuration node.
 *
 * This function takes a string containing YAML content and converts it into
 * a Config::Node object representing the parsed configuration.
 *
 * @param content The YAML content to parse.
 * @return Node The parsed configuration node.
 * @throws ParseError If the YAML content is invalid or cannot be parsed.
 */
CONFIG_FORMAT_YAML_EXPORT;
Node Parse(const std::string &content);

}
