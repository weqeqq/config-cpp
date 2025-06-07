
#pragma once 

#include <string>
#include <config/core/node.h>
#include <config/format/json/export.h>
#include <config/format/json/error.h>

namespace Config::JsonFormat {

/**
 * @brief Exception class for JSON parsing errors.
 *
 * This struct represents an error that occurs during JSON parsing. It inherits from
 * JsonError and constructs an error message with a "ParseError" prefix, combined with
 * a specific message provided during instantiation.
 */
struct ParseError : JsonError {
  /**
   * @brief Constructs a ParseError with a specific error message.
   *
   * Creates a ParseError instance by combining the "ParseError" prefix with the
   * provided message, passing the result to the base JsonError class.
   *
   * @param message A string view containing the specific error message describing the parsing issue.
   */
  ParseError(std::string_view message)
    : JsonError(Message("ParseError", message)) {}
};

/**
 * @brief Parses a JSON string into a node.
 *
 * This function takes a JSON-formatted string and converts it into a Node object,
 * which represents the parsed data structure.
 *
 * @param content The JSON-formatted string to parse.
 * @return A Node object representing the parsed JSON data.
 * @throws ParseError If the input string is not valid JSON or parsing fails.
 */
CONFIG_FORMAT_JSON_EXPORT;
Node Parse(const std::string &content);

}
