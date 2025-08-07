
#include <config-cpp/format/json/parse.h>

#include <format>
#include <nlohmann/json.hpp>
#include <string>
#include <type_traits>

using Json = nlohmann::json;
using JsonType = Json::value_t;

namespace Config::JsonFormat {
namespace {

Node Convert(const Json &json);

inline Node ConvertNull(const Json &input) { return Node(); }

inline Node ConvertString(const Json &input) {
  return Node(input.get<Node::String>());
}
inline Node ConvertBoolean(const Json &input) {
  return Node(input.get<Node::Boolean>());
}
inline Node ConvertInteger(const Json &input) {
  return Node(input.get<Node::Integer>());
}
inline Node ConvertFloating(const Json &input) {
  return Node(input.get<Node::Floating>());
}
inline Node ConvertSequence(const Json &input) {
  Node output;
  for (const auto &element : input) {
    output.Push(Convert(element));
  }
  return output;
}
inline Node ConvertObject(const Json &input) {
  Node output;
  for (const auto &[field, element] : input.items()) {
    output[field] = Convert(element);
  }
  return output;
}
Node Convert(const Json &input) {
  switch (input.type()) {
    case JsonType::null:
      return ConvertNull(input);
    case JsonType::string:
      return ConvertString(input);
    case JsonType::boolean:
      return ConvertBoolean(input);
    case JsonType::number_integer:
      return ConvertInteger(input);
    case JsonType::number_unsigned:
      return ConvertInteger(input);
    case JsonType::number_float:
      return ConvertFloating(input);
    case JsonType::array:
      return ConvertSequence(input);
    case JsonType::object:
      return ConvertObject(input);
    default:
      throw ParseError(std::format(
          "Undefined JsonType ({})",
          std::to_string(
              static_cast<std::underlying_type_t<JsonType>>(input.type()))));
  }
}
}  // namespace
Node Parse(std::string_view content) {
  try {
    return Convert(Json::parse(content));
  } catch (const Json::exception &error) {
    throw ParseError(error.what());
  }
}
}  // namespace Config::JsonFormat
