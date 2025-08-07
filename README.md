# config-cpp

[![Windows Build Status](https://github.com/weqeqq/config-cpp/actions/workflows/windows-latest.yml/badge.svg)](https://github.com/weqeqq/config-cpp/actions/workflows/build.yml)
[![Ubuntu Build Status](https://github.com/weqeqq/config-cpp/actions/workflows/ubuntu-latest.yml/badge.svg)](https://github.com/weqeqq/config-cpp/actions/workflows/build.yml)
[![Macos Build Status](https://github.com/weqeqq/config-cpp/actions/workflows/macos-latest.yml/badge.svg)](https://github.com/weqeqq/config-cpp/actions/workflows/build.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

`config-cpp` is a modern C++ library designed for easy and flexible management of configuration files. It provides a unified interface to load, parse, manipulate, and save configuration data in various formats, starting with JSON and YAML.

The core of the library is the `Config::Node` class, which represents configuration data as a tree structure, similar to how JSON or YAML values are structured. This allows for intuitive access and modification of configuration values.

## Features

- **Unified API**: A single, consistent API for working with different configuration formats.
- **Format Support**: Built-in support for JSON (`.json`) and YAML (`.yml`, `.yaml`).
- **Automatic Format Detection**: The library can automatically deduce the file format from its extension when loading or saving.
- **Intuitive Data Manipulation**: The `Config::Node` class offers an easy-to-use interface with `operator[]` for accessing nested values, and methods to check types and convert values.
- **Custom Type Conversions**: Seamlessly convert `Config::Node` objects to and from your own custom types.
- **Type Safety**: Provides methods for safe type conversions and checks.
- **Modern C++**: Built with C++20 features and integrated easily using CMake and `FetchContent`.

## Requirements

- C++20 compatible compiler
- CMake (>= 3.23)
- The following libraries are handled automatically via `FetchContent`:
  - [weqeqq/file-cpp](https://github.com/weqeqq/file-cpp)
  - [nlohmann/json](https://github.com/nlohmann/json)
  - [jbeder/yaml-cpp](https://github.com/jbeder/yaml-cpp)

## Installation

This library is designed to be easily integrated into your project using CMake's `FetchContent`. Add the following to your `CMakeLists.txt`:

```cmake
include(FetchContent)

FetchContent_Declare(
    config-cpp
    GIT_REPOSITORY https://github.com/weqeqq/config-cpp.git
    GIT_TAG main # Or a specific commit/tag
)

FetchContent_MakeAvailable(config-cpp)

# Link to your executable/library
target_link_libraries(your_target_name PRIVATE ConfigCpp::Static) # or ConfigCpp::Shared
```

## Usage

### Loading a Configuration File

Use `Config::Open` to load a configuration file. The format can be deduced from the file extension or specified explicitly.

```cpp
#include <config-cpp/config.h>
#include <iostream>

int main() {
    try {
        // Load from a JSON file (format is deduced from .json extension)
        Config::Node root = Config::Open("config.json");

        std::string name = root["name"].To<std::string>();
        int version = root["version"].To<int>();

        std::cout << "Name: " << name << ", Version: " << version << std::endl;

    } catch (const Config::Error& e) {
        std::cerr << "Configuration error: " << e.what() << std::endl;
    }
    return 0;
}
```

Assuming `config.json` contains:

```json
{
  "name": "MyApp",
  "version": 1,
  "features": {
    "enabled": true,
    "retries": 3
  }
}
```

### Accessing Data

The `Config::Node` provides an intuitive API for accessing data using `operator[]` for object keys and integer indices.

```cpp
// Assuming 'root' is a Config::Node loaded from the JSON above

// Access nested values by chaining calls
bool enabled = root["features"]["enabled"].To<bool>(); // true
int retries = root["features"]["retries"].To<int>();   // 3

// Use ValueOr to provide a default value if a key doesn't exist
std::string theme = root.ValueOr("theme", "default-theme"); // "default-theme"
```

### Modifying and Creating Configuration

You can easily modify existing nodes or build a new configuration tree from scratch.

```cpp
#include <config-cpp/config.h>

Config::Node root; // Create an empty (null) node

// It becomes an object on first key assignment
root["application"] = "MyCoolApp";
root["version"] = "2.0.1";
root["settings"]["theme"] = "dark";

// Create a sequence (array)
root["settings"]["ports"] = Config::Node::Sequence{8080, 8081, 9000};

// ports is now a sequence: [8080, 8081, 9000]
root["settings"]["ports"].Push(9001); // Add another element
```

### Saving a Configuration

Use `Config::Save` to write a `Config::Node` to a file. The format can be deduced from the extension or specified explicitly.

```cpp
#include <config-cpp/config.h>
#include <config-cpp/io/save.h>

// Assuming 'root' is the node created in the previous example

try {
    // Save to a YAML file (format deduced from .yaml extension)
    Config::Save("config.yaml", root);

    // Save to a JSON file explicitly
    Config::Save("config.json", root, Config::Format::Json);

} catch (const Config::Error& e) {
    std::cerr << "Failed to save configuration: " << e.what() << std::endl;
}
```

The resulting `config.yaml` would look like:

```yaml
application: MyCoolApp
version: "2.0.1"
settings:
  theme: dark
  ports:
    - 8080
    - 8081
    - 9000
    - 9001
```

### Custom Type Conversions

You can seamlessly convert `Config::Node` objects to and from your own custom types by providing conversion functions. This is supported via internal converters (defined inside your type) or external converters (defined via specialization).

#### From `Config::Node` to a Custom Type

To convert a `Config::Node` to your type, implement a `FromConfig` converter.

**Internal Converter:**

```cpp
#include <config-cpp/config.h>

struct Point {
    int x, y;

    // Internal converter from Config::Node to Point
    struct FromConfig {
        void operator()(const Config::Node& node, Point& point) const {
            point.x = node["x"].To<int>();
            point.y = node["y"].To<int>();
        }
    };
};

// Given a node representing { "x": 10, "y": 20 }
// Point p = node.To<Point>(); // p.x == 10, p.y == 20
```

**External Converter:** (If you cannot modify the type's definition)

```cpp
#include <config-cpp/config.h>

struct Point { int x, y; }; // Assume this cannot be modified

namespace Config {
template <>
struct From<Point> {
    void operator()(const Node& node, Point& point) const {
        point.x = node["x"].To<int>();
        point.y = node["y"].To<int>();
    }
};
} // namespace Config

// Usage is the same:
// Point p = node.To<Point>();
```

#### From a Custom Type to `Config::Node`

To convert your type to a `Config::Node`, implement a `ToConfig` converter.

**Internal Converter:**

```cpp
#include <config-cpp/config.h>

struct Point {
    int x, y;

    // Internal converter from Point to Config::Node
    struct ToConfig {
        void operator()(const Point& p, Config::Node& node) const {
            node["x"] = p.x;
            node["y"] = p.y;
        }
    };
};

Point p = {100, 200};
Config::Node root;
root["center"] = p; // Automatically uses ToConfig
// root["center"] is now: { "x": 100, "y": 200 }
```

**External Converter:**

```cpp
#include <config-cpp/config.h>

struct Point { int x, y; }; // Assume this cannot be modified

namespace Config {
template <>
struct To<Point> {
    void operator()(const Point& p, Node& node) const {
        node["x"] = p.x;
        node["y"] = p.y;
    }
};
} // namespace Config

// Usage is the same:
// Point p = {100, 200};
// root["center"] = p;
```

## Building

To build this project standalone (e.g., to run tests):

```bash
# Clone the repository
git clone https://github.com/weqeqq/config-cpp.git
cd config-cpp

# Configure and build the project
cmake -B build -S .
cmake --build build

# To run tests
cd build
ctest
```

You can control the build with the following CMake options:
- `CONFIG_CPP_BUILD_SHARED` (ON): Build the shared library.
- `CONFIG_CPP_BUILD_STATIC` (ON): Build the static library.
- `CONFIG_CPP_BUILD_TESTS` (ON): Build tests.
- `CONFIG_CPP_BUILD_DOCS` (ON): Build documentation.
- `CONFIG_CPP_YAML_SUPPORT` (ON): Enable YAML support.
- `CONFIG_CPP_JSON_SUPPORT` (ON): Enable JSON support.

## Dependencies

- [weqeqq/file-cpp](https://github.com/weqeqq/file-cpp) for file I/O operations.
- [nlohmann/json](https://github.com/nlohmann/json) for JSON parsing and serialization.
- [jbeder/yaml-cpp](https://github.com/jbeder/yaml-cpp) for YAML parsing and serialization.

These dependencies are managed automatically via `FetchContent` when you include `config-cpp` in your CMake project.

## License

This project is licensed under the MIT License.
