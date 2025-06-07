#pragma once 

#include <config/format/json/error.h>
#include <config/format/json/export.h>
#include <config/core/node.h>

namespace Config::JsonFormat {

/**
 * @brief Exception class for errors occurring during JSON dump operations.
 *
 * This struct inherits from JsonError and is used to represent errors
 * specific to the JSON dumping process.
 */
struct DumpError : JsonError {
  /**
   * @brief Constructs a DumpError with a specific error message.
   *
   * @param message A string view containing the error message.
   */
  DumpError(
    std::string_view message 
  ) : JsonError(Message("DumpError", message)) {}
};

/**
 * @brief Converts a configuration node to a JSON string.
 *
 * This function takes a configuration node and serializes it into
 * a JSON-formatted string.
 *
 * @param node The configuration node to be dumped.
 * @return A string containing the JSON representation of the node.
 * @throw DumpError If an error occurs during the dump process.
 */
CONFIG_FORMAT_JSON_EXPORT;
std::string Dump(const Node &node);
};
