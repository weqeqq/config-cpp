#include <config/format/json/parse.h>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Config::JsonFormat { 
namespace {

Node Convert(const json &input);

inline Node ConvertNull(const json &) {
  return Node();
}
inline Node ConvertString(const json &input) {
  return Node(input.get<Node::String>());
}
inline Node ConvertBoolean(const json &input) {
  return Node(input.get<Node::Boolean>());
}
inline Node ConvertInteger(const json &input) {
  return Node(input.get<Node::Integer>());
}
inline Node ConvertFloating(const json &input) {
  return Node(input.get<Node::Floating>());
}
inline Node ConvertSequence(const json &input) {
  Node output;

  for (const auto &element : input) {
    output.Push(Convert(element)); 
  }
  return output;
}
inline Node ConvertObject(const json &input) {
  Node output;

  for (const auto &[field, element] : input.items()) {
    output[field] = Convert(element);
  }
  return output;
}

Node Convert(const json &input) {
  using JsonT = json::value_t;

  switch (input.type()) {
    case JsonT::null            : return ConvertNull     (input);
    case JsonT::string          : return ConvertString   (input);
    case JsonT::boolean         : return ConvertBoolean  (input);
    case JsonT::number_integer  : return ConvertInteger  (input);
    case JsonT::number_unsigned : return ConvertInteger  (input);
    case JsonT::number_float    : return ConvertFloating (input);
    case JsonT::array           : return ConvertSequence (input);
    case JsonT::object          : return ConvertObject   (input);
    default: 
      return Node();
  }
}
}
Node Parse(const std::string &content) {
  try {
    return Convert(json::parse(content));
  } catch(const json::exception &error) {

    throw ParseError(error.what());
  }
}
}

