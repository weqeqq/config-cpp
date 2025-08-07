/**
 * @file yaml.h
 * @brief A convenience header for YAML format support.
 *
 * This header includes all necessary components for handling YAML data within
 * the configuration library. It provides functions for both parsing YAML
 * strings into `Config::Node` objects and serializing `Config::Node` objects
 * into YAML strings.
 *
 * By including this single header, you gain access to:
 * - `Config::YamlFormat::Parse()` for deserialization.
 * - `Config::YamlFormat::Dump()` for serialization.
 * - Associated exception classes like `Config::YamlFormat::ParseError` and
 *   `Config::YamlFormat::DumpError`.
 */
#include <config-cpp/format/yaml/dump.h>
#include <config-cpp/format/yaml/parse.h>
