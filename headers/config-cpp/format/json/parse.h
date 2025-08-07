/**
 * @file parse.h
 * @brief Defines the JSON parsing functionality for the config-cpp library.
 *
 * This file provides the `Config::JsonFormat::Parse` function, which allows
 * for parsing a JSON string represented as a `std::string_view` into a
 * `Config::Node`. It also defines the `Config::JsonFormat::ParseError`
 * exception, which is thrown when parsing errors are encountered.
 */
#pragma once

#include <config-cpp/export.h>
#include <config-cpp/format/json/error.h>
#include <config-cpp/node.h>

#include <string_view>

namespace Config::JsonFormat {

/**
 * @brief Exception thrown for JSON parsing errors.
 *
 * This exception is thrown by `Parse` when it encounters an error while
 * parsing a JSON string.
 */
struct ParseError : JsonError {
  using JsonError::JsonError;
};

/**
 * @brief Parses a JSON string into a `Config::Node`.
 *
 * This function takes a string view containing JSON data and attempts to parse
 * it into a hierarchical `Config::Node` structure.
 *
 * @param string The `std::string_view` containing the JSON data to be parsed.
 * @return A `Config::Node` representing the root of the parsed JSON document.
 * @throws ParseError if the input string is not valid JSON.
 */
CONFIG_CPP_EXPORT
Node Parse(std::string_view string);

};  // namespace Config::JsonFormat
