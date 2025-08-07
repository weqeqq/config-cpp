/**
 * @file dump.h
 * @brief Provides functionality to serialize a configuration node into a YAML
 * string.
 *
 * This file contains the `Config::YamlFormat::Dump` function, which converts a
 * `Config::Node` object into its YAML string representation. It also defines
 * the `Config::YamlFormat::DumpError` exception for errors that may occur
 * during the serialization process.
 */
#pragma once

#include <config-cpp/export.h>
#include <config-cpp/format/yaml/error.h>
#include <config-cpp/node.h>

#include <string>

namespace Config::YamlFormat {

/**
 * @brief An exception class for errors during YAML serialization.
 *
 * This exception is thrown when an error occurs while dumping a `Config::Node`
 * to a YAML string. It inherits from `Config::YamlError`.
 */
struct DumpError : YamlError {
  using YamlError::YamlError;
};

/**
 * @brief Serializes a configuration node into a YAML string.
 *
 * This function takes a `Config::Node` and converts it into its corresponding
 * YAML string representation.
 *
 * @param[in] node The configuration node to serialize.
 * @return A string containing the YAML representation of the node.
 * @throw Config::YamlFormat::DumpError if serialization fails.
 */
CONFIG_CPP_EXPORT
std::string Dump(const Node &node);

}  // namespace Config::YamlFormat
