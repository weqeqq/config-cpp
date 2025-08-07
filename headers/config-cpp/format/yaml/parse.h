/**
 * @file parse.h
 * @brief Provides functionality for parsing YAML data.
 *
 * This file contains the necessary declarations for parsing YAML-formatted
 * strings into `Config::Node` objects. It exposes a `Parse` function to
 * perform the conversion and defines a `ParseError` exception that can be
 * thrown if the input string is not valid YAML.
 */
#pragma once

#include <config-cpp/export.h>
#include <config-cpp/format/yaml/error.h>
#include <config-cpp/node.h>

#include <string_view>

namespace Config::YamlFormat {

/**
 * @brief An exception class for YAML parsing errors.
 *
 * This exception is thrown when an error occurs during the parsing of a YAML
 * string. It inherits from `Config::YamlError`.
 */
struct ParseError : YamlError {
  using YamlError::YamlError;
};

/**
 * @brief Parses a YAML-formatted string into a `Node`.
 *
 * @param string The YAML-formatted string to parse.
 * @return A `Node` object representing the root of the parsed YAML data.
 * @throw ParseError If the input string is not a valid YAML document.
 */
CONFIG_CPP_EXPORT
Node Parse(std::string_view string);

};  // namespace Config::YamlFormat
