/**
 * @file format.h
 * @brief A convenience header for all supported configuration formats.
 *
 * This header aggregates all format-related functionality of the config-cpp
 * library. By including this single file, you get access to all supported
 * formats, like JSON and YAML, and the `Config::Format` enum for specifying
 * a format type.
 *
 * It includes the following headers:
 * - `config-cpp/format/json.h`: For JSON support.
 * - `config-cpp/format/yaml.h`: For YAML support.
 */
#pragma once

#include <config-cpp/format/json.h>
#include <config-cpp/format/yaml.h>

namespace Config {
/**
 * @brief Enumerates the supported data formats.
 */
enum class Format {
  Json, /**< The JSON format. */
  Yaml  /**< The YAML format. */
};
}  // namespace Config
