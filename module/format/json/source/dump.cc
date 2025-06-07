
#include <cassert>
#include <config/format/json/dump.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Config::JsonFormat {
namespace {

json Convert(const Node &node);

json ConvertNull(const Node &) {
  return json(json::value_t::null);
}
json ConvertString(const Node &node) {
  return json(node.AsString());
}
json ConvertBoolean(const Node &node) {
  return json(node.AsBoolean());
}
json ConvertInteger(const Node &node) {
  return json(node.AsInteger());
}
json ConvertFloating(const Node &node) {
  return json(node.AsFloating());
}
json ConvertSequence(const Node &node) {
  json output(json::value_t::array);

  for (const auto &element : node.AsSequence()) {
    output.push_back(Convert(element));
  }
  return output;
}
json ConvertObject(const Node &node) {
  json output(json::value_t::object);

  for (const auto &[field, element] : node.AsObject()) {
    output[field] = Convert(element);
  }
  return output;
}
json Convert(const Node &node) {
  switch (node.Type()) {
    case NodeType::Null     : return ConvertNull     (node);
    case NodeType::String   : return ConvertString   (node);
    case NodeType::Boolean  : return ConvertBoolean  (node);
    case NodeType::Integer  : return ConvertInteger  (node);
    case NodeType::Floating : return ConvertFloating (node);
    case NodeType::Sequence : return ConvertSequence (node);
    case NodeType::Object   : return ConvertObject   (node);
    default:
      throw DumpError("Undefined NodeType");
  }
}
}
std::string Dump(const Node &node) {
  try {
    return Convert(node).dump(4);
  } catch(const json::exception &error) {

    throw DumpError(error.what());
  }
}
}
