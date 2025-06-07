
#include <config/format/yaml/dump.h>
#include <yaml-cpp/yaml.h>

namespace Config::YamlFormat {
namespace {

YAML::Node Convert(const Node &node);

inline YAML::Node ConvertNull(const Node &) {
  return YAML::Node(YAML::NodeType::Null);
}
inline YAML::Node ConvertString(const Node &input) {
  return YAML::Node(input.AsString());
}
inline YAML::Node ConvertBoolean(const Node &input) {
  return YAML::Node(input.AsBoolean());
}
inline YAML::Node ConvertInteger(const Node &input) {
  return YAML::Node(input.AsInteger());
}
inline YAML::Node ConvertFloating(const Node &input) {
  return YAML::Node(input.AsFloating());
}
inline YAML::Node ConvertSequence(const Node &input) {
  YAML::Node output(YAML::NodeType::Sequence);

  for (const auto &element : input.AsSequence()) {
    output.push_back(Convert(element));
  }
  return output;
}
inline YAML::Node ConvertObject(const Node &input) {
  YAML::Node output(YAML::NodeType::Map);

  for (const auto &[field, element] : input.AsObject()) {
    output[field] = Convert(element);
  }
  return output;
}
YAML::Node Convert(const Node &node) {
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
    return YAML::Dump(Convert(node));
  } catch(const YAML::Exception &error) {

    throw DumpError(error.what());
  }
}
}
