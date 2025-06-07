
#include "yaml-cpp/node/type.h"
#include <config/format/yaml/parse.h>
#include <yaml-cpp/yaml.h>

namespace Config::YamlFormat {
namespace {

Node Convert(const YAML::Node &input);

inline Node ConvertNull(const YAML::Node &input) {
  return Node();
}
inline Node ConvertSequence(const YAML::Node &input) {
  Node output;

  for (const auto &element : input) {
    output.Push(Convert(element));
  }
  return output;
}
inline Node ConvertMap(const YAML::Node &input) {
  Node output;

  for (const auto &element : input) {
    output[element.first.as<Node::String>()] = Convert(element.second);
  }
  return output;
}
inline Node ConvertScalar(const YAML::Node &input) {
  try {
    return Node(input.as<Node::String>());
  } catch(const YAML::BadConversion &) {
    try {
      return Node(input.as<Node::Integer>());
    } catch(const YAML::BadConversion &) {
      try {
        return Node(input.as<Node::Floating>());
      } catch(const YAML::BadConversion &) {

        return Node(input.as<Node::Boolean>());
      }
    }
  }
}
Node Convert(const YAML::Node &input) {
  switch (input.Type()) {
    case YAML::NodeType::Null     : return ConvertNull     (input);
    case YAML::NodeType::Scalar   : return ConvertScalar   (input);
    case YAML::NodeType::Sequence : return ConvertSequence (input);
    case YAML::NodeType::Map      : return ConvertMap      (input);
    default:
      throw ParseError("UndefinedNodeType");
  }
}
}
Node Parse(const std::string &input) {
  try {
    return Convert(YAML::Load(input));
  } catch(const YAML::Exception &error) {

    throw ParseError(error.what());
  }
}
}
