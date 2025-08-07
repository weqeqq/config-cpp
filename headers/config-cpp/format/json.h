/**
 * @file json.h
 * @brief Convenience header for JSON format support in the config-cpp library.
 *
 * This header aggregates the core functionalities for handling the JSON format,
 * providing a single include for both parsing and serialization (dumping).
 * By including this file, you get access to all JSON-related features of the
 * library.
 *
 * It includes the following headers:
 * - `config-cpp/format/json/parse.h`: Provides `Config::JsonFormat::Parse()`
 *   for parsing a JSON string into a `Config::Node`.
 * - `config-cpp/format/json/dump.h`: Provides `Config::JsonFormat::Dump()` for
 *   serializing a `Config::Node` into a JSON string.
 *
 * This simplifies usage by not requiring developers to include individual
 * feature headers.
 */
#include <config-cpp/format/json/dump.h>
#include <config-cpp/format/json/parse.h>
