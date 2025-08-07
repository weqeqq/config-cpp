
#include <config-cpp/format/json/dump.h>

#include <nlohmann/json.hpp>
#include <type_traits>

using Json = nlohmann::json;
using JsonType = Json::value_t;

namespace Config::JsonFormat {
namespace {

Json Convert(const Node &node);

Json ConvertNull(const Node &) { return Json(JsonType::null); }
Json ConvertString(const Node &node) { return Json(node.AsString()); }
Json ConvertBoolean(const Node &node) { return Json(node.AsBoolean()); }
Json ConvertInteger(const Node &node) { return Json(node.AsInteger()); }
Json ConvertFloating(const Node &node) { return Json(node.AsFloating()); }
Json ConvertSequence(const Node &node) {
  Json output(JsonType::array);

  for (const auto &element : node.AsSequence()) {
    output.push_back(Convert(element));
  }
  return output;
}
Json ConvertObject(const Node &node) {
  Json output(JsonType::object);

  for (const auto &[field, element] : node.AsObject()) {
    output[field] = Convert(element);
  }
  return output;
}
Json Convert(const Node &node) {
  switch (node.Type()) {
    case NodeType::Null:
      return ConvertNull(node);
    case NodeType::String:
      return ConvertString(node);
    case NodeType::Boolean:
      return ConvertBoolean(node);
    case NodeType::Integer:
      return ConvertInteger(node);
    case NodeType::Floating:
      return ConvertFloating(node);
    case NodeType::Sequence:
      return ConvertSequence(node);
    case NodeType::Object:
      return ConvertObject(node);
    default:
      throw DumpError(std::format(
          "Undefined NodeType ({})",
          std::to_string(
              static_cast<std::underlying_type_t<NodeType>>(node.Type()))));
  }
}
}  // namespace
std::string Dump(const Node &node) {
  try {
    return Convert(node).dump(4);
  } catch (const Json::exception &error) {
    throw DumpError(error.what());
  }
}
}  // namespace Config::JsonFormat
