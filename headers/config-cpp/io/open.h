/**
 * @file open.h
 * @brief Defines the `Open` function object for loading configuration files.
 *
 * This file provides utilities to open and parse configuration files. The main
 * component is the `Config::Open` function object, which can load a file
 * from a path and parse it according to a specified format or a format
 * deduced from the file extension.
 *
 * It also defines `Config::OpenError` for exceptions that occur during these
 * operations.
 */
#pragma once

#include <config-cpp/format.h>
#include <config-cpp/io/error.h>
#include <file-cpp/file.h>

#include <filesystem>

namespace Config {

/**
 * @brief An exception thrown for errors during file opening.
 *
 * This exception is thrown by `Config::Open` when it fails, for example,
 * because the format is unknown or cannot be deduced from the file extension.
 */
struct OpenError : IoError {
  using IoError::IoError;
};
namespace {
// Internal implementation for the Config::Open function object.
struct OpenFn {
  Node operator()(const std::filesystem::path &path, Format format) const {
    switch (format) {
      case Format::Json:
        return JsonFormat::Parse(File::ReadString(path));
      case Format::Yaml:
        return YamlFormat::Parse(File::ReadString(path));
      default:
        throw OpenError("Undefined format");
    }
  }
  Node operator()(const std::filesystem::path &path) const {
    auto extension = path.extension();
    if (extension == ".json") {
      return operator()(path, Format::Json);
    }
    if (extension == ".yml" || extension == ".yaml") {
      return operator()(path, Format::Yaml);
    }
    throw OpenError("Undefined format");
  }
};
}  // namespace
/**
 * @brief A function object for opening and parsing configuration files.
 *
 * This object can be called like a function to load a configuration file from a
 * given path into a `Config::Node`. It provides two overloads for this purpose.
 *
 * @par Overload 1: Explicit format
 * `Node operator()(const std::filesystem::path &path, Format format) const`
 *
 * Loads and parses a file using an explicitly specified format.
 *
 * @param path The path to the configuration file.
 * @param format The format to use for parsing (e.g., `Format::Json`).
 * @return A `Config::Node` representing the root of the parsed configuration.
 * @throws OpenError if the specified `format` is unsupported.
 * @throws Exceptions from `File::ReadString` on file I/O errors.
 * @throws Exceptions from the corresponding parser on malformed file content.
 *
 * @par Overload 2: Format deduction
 * `Node operator()(const std::filesystem::path &path) const`
 *
 * Loads and parses a file, deducing its format from the file extension:
 * - `.json`: Assumes `Format::Json`.
 * - `.yml` or `.yaml`: Assumes `Format::Yaml`.
 *
 * @param path The path to the configuration file.
 * @return A `Config::Node` representing the root of the parsed configuration.
 * @throws OpenError if the format cannot be deduced from the file extension.
 * @throws Exceptions from `File::ReadString` on file I/O errors.
 * @throws Exceptions from the corresponding parser on malformed file content.
 *
 * @par Example
 * @code
 * try {
 *     // Open with format deduction
 *     Config::Node root = Config::Open("config.json");
 *
 *     // Open with explicit format
 *     Config::Node root_explicit = Config::Open("config.yml",
 * Config::Format::Yaml); } catch (const Config::Error& e) {
 *     // Handle configuration-related errors
 *     std::cerr << "Config error: " << e.what() << std::endl;
 * } catch (...) {
 *     // Handle other errors, e.g., file not found
 * }
 * @endcode
 */
inline constexpr auto Open = OpenFn();

}  // namespace Config
