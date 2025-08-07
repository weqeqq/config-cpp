/**
 * @mainpage config-cpp
 *
 * @section intro_sec Introduction
 *
 * `config-cpp` is a modern C++ library designed for easy and flexible
 * management of configuration files. It provides a unified interface to
 * load, parse, manipulate, and save configuration data in various formats,
 * starting with JSON and YAML.
 *
 * The core of the library is the `Config::Node` class, which represents
 * configuration data as a tree structure, similar to how JSON or YAML
 * values are structured. This allows for intuitive access and modification
 * of configuration values.
 *
 * @section features_sec Features
 *
 * - **Unified API**: A single, consistent API for working with different
 *   configuration formats.
 * - **Format Support**: Built-in support for JSON (`.json`) and YAML
 *   (`.yml`, `.yaml`).
 * - **Automatic Format Detection**: The library can automatically deduce the
 *   file format from its extension when loading or saving.
 * - **Intuitive Data Manipulation**: The `Config::Node` class offers an
 *   easy-to-use interface with `operator[]` for accessing nested values,
 *   and methods to check types and convert values.
 * - **Type Safety**: Provides methods for safe type conversions and checks.
 * - **Modern C++**: Built with C++20 features.
 *
 * @section usage_sec Basic Usage
 *
 * The library is designed to be straightforward to use. Here are some
 * common scenarios.
 *
 * @subsection load_config Loading a Configuration File
 *
 * Use `Config::Open` to load a configuration file. The format can be deduced
 * from the file extension or specified explicitly.
 *
 * @code{.cpp}
 * #include <config-cpp/config.h>
 * #include <iostream>
 *
 * int main() {
 *     try {
 *         // Load from a JSON file (format is deduced)
 *         Config::Node root = Config::Open("config.json");
 *
 *         // You can also specify the format explicitly
 *         // Config::Node root = Config::Open("my_config",
 * Config::Format::Json);
 *
 *         std::string name = root["name"].To<std::string>();
 *         int version = root["version"].To<int>();
 *
 *         std::cout << "Name: " << name << ", Version: " << version <<
 * std::endl;
 *
 *     } catch (const Config::Error& e) {
 *         std::cerr << "Configuration error: " << e.what() << std::endl;
 *     }
 *     return 0;
 * }
 * @endcode
 *
 * Assuming `config.json` contains:
 * @code{.json}
 * {
 *   "name": "MyApp",
 *   "version": 1,
 *   "features": {
 *     "enabled": true,
 *     "retries": 3
 *   }
 * }
 * @endcode
 *
 * @subsection access_data Accessing Data
 *
 * The `Config::Node` provides an intuitive API for accessing data using
 * `operator[]` for object keys and integer indices. You can chain these calls
 * to access nested data.
 *
 * @code{.cpp}
 * #include <config-cpp/config.h>
 *
 * // Assuming 'root' is a Config::Node loaded from the JSON above
 *
 * // Access nested values
 * bool enabled = root["features"]["enabled"].To<bool>(); // true
 * int retries = root["features"]["retries"].To<int>();   // 3
 *
 * // Using ValueOr to provide a default value
 * std::string theme = root.ValueOr("theme", "default-theme");
 * @endcode
 *
 * @subsection modify_data Modifying and Creating Configuration
 *
 * You can easily modify existing nodes or build a new configuration tree
 * from scratch.
 *
 * @code{.cpp}
 * #include <config-cpp/config.h>
 *
 * Config::Node root; // Create an empty (null) node
 *
 * // It becomes an object on first key assignment
 * root["application"] = "MyCoolApp";
 * root["version"] = "2.0.1";
 * root["settings"]["theme"] = "dark";
 * root["settings"]["ports"] = Config::Node::Sequence{8080, 8081, 9000};
 *
 * // ports is now a sequence: [8080, 8081, 9000]
 * root["settings"]["ports"].Push(9001); // Add another port
 * @endcode
 *
 * @subsection custom_types Custom Type Conversions
 *
 * You can seamlessly convert `Config::Node` objects to and from your own
 * custom types by providing conversion functions. This allows for cleaner, more
 * expressive code when dealing with complex configuration structures.
 *
 * The library discovers converters in two ways:
 * - **Internal Converters**: A `ToConfig` or `FromConfig` struct defined
 *   within the custom type's class or struct.
 * - **External Converters**: A specialization of `Config::To<T>` or
 *   `Config::From<T>` for the custom type `T`.
 *
 * ### Converting from a Node to a Custom Type (`FromConfig`)
 *
 * To convert a `Config::Node` to your type, implement a `FromConfig` converter.
 *
 * @par Internal Converter
 * @code{.cpp}
 * #include <config-cpp/config.h>
 *
 * struct Point {
 *     int x, y;
 *
 *     // Internal converter from Config::Node to Point
 *     struct FromConfig {
 *         void operator()(const Config::Node& node, Point& point) const {
 *             point.x = node["x"].To<int>();
 *             point.y = node["y"].To<int>();
 *         }
 *     };
 * };
 *
 * // Given a node representing { "x": 10, "y": 20 }
 * // Point p = node.To<Point>(); // p.x == 10, p.y == 20
 * @endcode
 *
 * @par External Converter
 * If you cannot modify the type's definition, provide an external converter by
 * specializing `Config::From`.
 *
 * @code{.cpp}
 * #include <config-cpp/config.h>
 *
 * struct Point { int x, y; }; // Assume this cannot be modified
 *
 * namespace Config {
 * template <>
 * struct From<Point> {
 *     void operator()(const Node& node, Point& point) const {
 *         point.x = node["x"].To<int>();
 *         point.y = node["y"].To<int>();
 *     }
 * };
 * } // namespace Config
 *
 * // Usage is the same:
 * // Point p = node.To<Point>();
 * @endcode
 *
 * ### Converting from a Custom Type to a Node (`ToConfig`)
 *
 * To convert your type to a `Config::Node`, implement a `ToConfig` converter.
 *
 * @par Internal Converter
 * @code{.cpp}
 * #include <config-cpp/config.h>
 *
 * struct Point {
 *     int x, y;
 *
 *     // Internal converter from Point to Config::Node
 *     struct ToConfig {
 *         void operator()(const Point& p, Config::Node& node) const {
 *             node["x"] = p.x;
 *             node["y"] = p.y;
 *         }
 *     };
 * };
 *
 * Point p = {100, 200};
 * Config::Node root;
 * root["center"] = p; // Automatically uses ToConfig
 * // root["center"] is now: { "x": 100, "y": 200 }
 * @endcode
 *
 * @par External Converter
 * An external converter can be provided by specializing `Config::To`.
 *
 * @code{.cpp}
 * #include <config-cpp/config.h>
 *
 * struct Point { int x, y; }; // Assume this cannot be modified
 *
 * namespace Config {
 * template <>
 * struct To<Point> {
 *     void operator()(const Point& p, Node& node) const {
 *         node["x"] = p.x;
 *         node["y"] = p.y;
 *     }
 * };
 * } // namespace Config
 *
 * // Usage is the same:
 * // Point p = {100, 200};
 * // root["center"] = p;
 * @endcode
 *
 * @subsection save_config Saving a Configuration
 *
 * Use `Config::Save` to write a `Config::Node` to a file. Again, the format can
 * be deduced or specified explicitly.
 *
 * @code{.cpp}
 * #include <config-cpp/config.h>
 *
 * try {
 *     // Save to a YAML file (format deduced from extension)
 *     Config::Save("config.yaml", root);
 *
 *     // Save to a JSON file explicitly
 *     Config::Save("config.json", root, Config::Format::Json);
 *
 * } catch (const Config::Error& e) {
 *     std::cerr << "Failed to save configuration: " << e.what() << std::endl;
 * }
 * @endcode
 *
 * The resulting `config.yaml` would look like:
 * @code{.yaml}
 * application: MyCoolApp
 * version: "2.0.1"
 * settings:
 *   theme: dark
 *   ports:
 *     - 8080
 *     - 8081
 *     - 9000
 *     - 9001
 * @endcode
 *
 * @section components_sec Core Components
 *
 * - `Config::Node`: The central class for representing any configuration data.
 * - `Config::Open`: A function object to load configuration from a file.
 * - `Config::Save`: A function object to save configuration to a file.
 * - `Config::Format`: An enum (`Json`, `Yaml`) to specify data formats.
 *
 */
/**
 * @file config.h
 * @brief The primary include file for the config-cpp library.
 *
 * This file serves as the main entry point for using the `config-cpp` library.
 * By including this single header, users gain access to all the core
 * functionalities, such as `Config::Node`, `Config::Open`, and `Config::Save`.
 *
 * It is recommended to include this file for most use cases.
 */
#include <config-cpp/format.h>
#include <config-cpp/io.h>
