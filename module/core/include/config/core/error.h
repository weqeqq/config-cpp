
#pragma once 

#include <format>
#include <stdexcept>
#include <string_view>

namespace Config {

class Error : public std::runtime_error {
protected:
  Error(
    std::string_view message

  ) : std::runtime_error(Namespace("Config", message)) {}

  static std::string Namespace(
    std::string_view prefix,
    std::string_view message
  ) {
    return std::format("{}::{}", prefix, message);
  }
  static std::string Message(
    std::string_view prefix,
    std::string_view message
  ) {
    return std::format("{}: {}", prefix, message);
  }
};

};
