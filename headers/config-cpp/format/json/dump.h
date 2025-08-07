/**
 * @file dump.h
 * @brief Defines the JSON serialization (dumping) functionality for the
 * config-cpp library.
 *
 * This file provides the `Config::JsonFormat::Dump` function, which converts a
 * `Config::Node` object into its JSON string representation. It also defines
 * the `Config::JsonFormat::DumpError` exception for errors that occur during
 * this process.
 */
#pragma once

#include <config-cpp/export.h>
#include <config-cpp/format/json/error.h>
#include <config-cpp/node.h>

#include <string>

namespace Config::JsonFormat {

/**
 * @brief Exception for JSON serialization errors.
 *
 * This exception is thrown when an error occurs during the conversion of a
 * `Config::Node` to a JSON string.
 */
struct DumpError : JsonError {
  using JsonError::JsonError;
};

/**
 * @brief Serializes a `Config::Node` into a JSON string.
 *
 * This function traverses the given `Config::Node` and constructs its
 * corresponding JSON representation as a string.
 *
 * @param node The `Config::Node` to be serialized.
 * @return A `std::string` containing the JSON representation of the node.
 * @throws DumpError If an error occurs during serialization.
 */
CONFIG_CPP_EXPORT
std::string Dump(const Node &node);

}  // namespace Config::JsonFormat
