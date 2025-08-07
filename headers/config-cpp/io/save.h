/**
 * @file save.h
 * @brief Defines a function for saving configuration data.
 *
 * This file provides the `Config::Save` function object, which is used to
 * serialize a `Config::Node` and write it to a file. The format can be
 * specified explicitly or inferred from the file extension.
 *
 * Example usage:
 *
 * @code
 * #include <config-cpp/config.h>
 * #include <config-cpp/io/save.h>
 *
 * // ... create a Config::Node
 * Config::Node root;
 * root["key"] = "value";
 *
 * // Save to a JSON file (format deduced from extension)
 * Config::Save("config.json", root);
 *
 * // Save to a YAML file (format specified explicitly)
 * Config::Save("config.data", root, Config::Format::Yaml);
 * @endcode
 *
 * The `Config::SaveError` exception is thrown for issues during the save
 * operation, such as an unsupported format.
 */
#pragma once

#include <config-cpp/format.h>
#include <config-cpp/io/error.h>
#include <file-cpp/file.h>

#include <filesystem>

namespace Config {

/**
 * @brief An exception thrown for errors during a save operation.
 *
 * This is thrown by `Config::Save` if an unsupported format is specified.
 */
struct SaveError : IoError {
  using IoError::IoError;
};
namespace {
struct SaveFn {
  void operator()(const std::filesystem::path &path, const Node &node,
                  Format format) const {
    switch (format) {
      case Format::Json:
        return File::WriteString(path, JsonFormat::Dump(node));
      case Format::Yaml:
        return File::WriteString(path, YamlFormat::Dump(node));
      default:
        throw SaveError("Undefined format");
    }
  }
  void operator()(const std::filesystem::path &path, const Node &node) const {
    auto extension = path.extension();
    if (extension == ".json") {
      return operator()(path, node, Format::Json);
    }
    if (extension == ".yml" || extension == ".yaml") {
      return operator()(path, node, Format::Yaml);
    }
    throw SaveError("Undefined format");
  }
};
}  // namespace
/**
 * @brief A function object for saving a `Config::Node` to a file.
 *
 * This function object serializes a `Config::Node` and writes it to the
 * specified file path. The format can be specified explicitly or inferred from
 * the file extension.
 *
 * @throws Config::SaveError if the format is unsupported or an I/O error
 * occurs.
 *
 * @par Example
 * @code
 * Config::Node root;
 * root["key"] = "value";
 *
 * // Save to a JSON file (format deduced from extension)
 * Config::Save("config.json", root);
 *
 * // Save to a YAML file (format specified explicitly)
 * Config::Save("config.data", root, Config::Format::Yaml);
 * @endcode
 */
inline constexpr auto Save = SaveFn();
}  // namespace Config
