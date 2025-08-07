/**
 * @file node.h
 * @brief Defines the Node class for handling configuration data with variant
 * types.
 *
 * This header provides the Node class, which is used to represent configuration
 * data in a type-safe manner using std::variant. It supports various data types
 * such as null, string, boolean, integer, floating-point, sequence, and object.
 * The class includes methods for type checking, conversion, and accessing
 * nested fields.
 */

#pragma once

#include <config-cpp/error.h>

#include <cstdint>
#include <format>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

#ifdef __GNUC__
#include <cxxabi.h>
#endif

namespace Config {

class Node;

/**
 * @struct From
 * @brief A tag for specifying a conversion from a user-defined type to a Node.
 * @tparam T The type to convert from.
 */
template <typename T>
struct From {};

/**
 * @struct To
 * @brief A tag for specifying a conversion from a Node to a user-defined type.
 * @tparam T The type to convert to.
 */
template <typename T>
struct To {};

/// @cond INTERNAL
namespace {
namespace trait {

template <typename T>
static constexpr bool IsString =
    std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view> ||
    std::is_same_v<T, const char *>;

template <typename T>
concept String = IsString<T>;

template <typename T>
static constexpr bool IsBoolean = std::is_same_v<T, bool>;

template <typename T>
concept Boolean = IsBoolean<T>;

template <typename T>
static constexpr bool IsInteger = std::is_integral_v<T>;

template <typename T>
concept Integer = IsInteger<T>;

template <typename T>
static constexpr bool IsFloating = std::is_floating_point_v<T>;

template <typename T>
concept Floating = IsFloating<T>;

template <typename T>
static constexpr bool IsSequence = std::is_same_v<T, std::vector<Node>>;

template <typename T>
concept Sequence = IsSequence<T>;

template <typename T>
static constexpr bool IsObject =
    std::is_same_v<T, std::unordered_map<std::string, Node>>;

template <typename T>
concept Object = IsObject<T>;

template <typename ToConfig, typename Input>
concept ToConfigConverter = requires(const Input &input, Node &node) {
  { ToConfig()(input, node) } -> std::same_as<void>;
};

template <typename Input>
concept HasExternalToConfigConverter =
    ToConfigConverter<Config::From<Input>, Input>;

template <typename Input>
concept HasInternalToConfigConverter =
    ToConfigConverter<typename Input::ToConfig, Input>;

template <typename Input>
concept HasToConfigConverter =
    HasExternalToConfigConverter<Input> || HasInternalToConfigConverter<Input>;

template <HasExternalToConfigConverter Input>
inline void ConvertToConfig(const Input &input, Node &node) {
  Config::From<Input>()(input, node);
}
template <HasInternalToConfigConverter Input>
inline void ConvertToConfig(const Input &input, Node &node) {
  typename Input::ToConfig()(input, node);
}

template <typename FromConfig, typename Output>
concept FromConfigConverter = requires(const Node &node, Output &output) {
  { FromConfig()(node, output) } -> std::same_as<void>;
};

template <typename Output>
concept HasExternalFromConfigConverter =
    FromConfigConverter<Config::To<Output>, Output>;

template <typename Output>
concept HasInternalFromConfigConverter =
    FromConfigConverter<typename Output::FromConfig, Output>;

template <typename Output>
concept HasFromConfigConverter = HasExternalFromConfigConverter<Output> ||
                                 HasInternalFromConfigConverter<Output>;

template <HasExternalFromConfigConverter Output>
void ConvertFromConfig(const Node &node, Output &output) {
  Config::To<Output>()(node, output);
}
template <HasInternalFromConfigConverter Output>
void ConvertFromConfig(const Node &node, Output &output) {
  typename Output::FromConfig()(node, output);
}
}  // namespace trait
}  // namespace
/// @endcond

/**
 * @class NodeError
 * @brief Base class for errors related to Node operations.
 *
 * Inherits from the Error class and provides a constructor for creating error
 * messages specific to Node operations.
 */
class NodeError : public Error {
 public:
  /**
   * @brief Constructs a NodeError with a given message.
   * @param message A string view containing the error message.
   */
  NodeError(std::string_view message) : Error(message) {}
};

/**
 * @brief Enumerates the possible types of a node in a data structure.
 *
 * This enum class defines the different types a node can have, such as null,
 * string, boolean, integer, floating-point, sequence, or object. It is used to
 * identify the type of data stored in a node.
 */
enum class NodeType {
  Null,     /**< Represents a null or undefined node.                  */
  String,   /**< Represents a string value.                            */
  Boolean,  /**< Represents a boolean value (true or false).           */
  Integer,  /**< Represents an integer value.                          */
  Floating, /**< Represents a floating-point value.                    */
  Sequence, /**< Represents a sequence (e.g., array or list) of nodes. */
  Object    /**< Represents an object (e.g., key-value map) of nodes.  */
};

/**
 * @class Node
 * @brief A class representing a configuration node with variant data types.
 *
 * The Node class encapsulates a std::variant that can hold different types of
 * data: null, string, boolean, integer, floating-point, sequence, or object. It
 * provides methods for type checking, conversion, and accessing nested data.
 */
class Node {
 public:
  /// Type alias for null values (std::monostate).
  using Null = std::monostate;
  /// Type alias for string values (std::string).
  using String = std::string;
  /// Type alias for boolean values.
  using Boolean = bool;
  /// Type alias for integer values (std::int64_t).
  using Integer = std::int64_t;
  /// Type alias for floating-point values (double).
  using Floating = double;
  /// Type alias for sequence of nodes (std::vector<Node>).
  using Sequence = std::vector<Node>;
  /// Type alias for object mapping strings to nodes
  /// (std::unordered_map<std::string, Node>).
  using Object = std::unordered_map<std::string, Node>;

  /// Variant type that can hold any of the supported data types.
  using Value =
      std::variant<Null, String, Boolean, Integer, Floating, Sequence, Object>;

  /**
   * @brief Default constructor initializing the Node to Null.
   */
  Node() : value_(Null()) {}

  /**
   * @brief Constructs a Node from a string-like type.
   *
   * This constructor accepts any type that satisfies the `trait::String`
   * concept, which includes `std::string`, `std::string_view`, and
   * `const char*`.
   *
   * @tparam S A type that satisfies the `trait::String` concept.
   * @param string The string value to initialize the Node with.
   */
  template <trait::String S>
  Node(S string) : value_(String(std::move(string))) {}

  /**
   * @brief Constructs a Node from a boolean.
   * @param boolean The boolean value to initialize the Node with.
   */
  Node(Boolean boolean) : value_(boolean) {}

  /**
   * @brief Constructs a Node from an integer.
   * @tparam I An integral type.
   * @param integer The integer value to initialize the Node with.
   */
  template <trait::Integer I>
  Node(I integer) : value_(Integer(integer)) {}

  /**
   * @brief Constructs a Node from a floating-point value.
   * @tparam F A floating-point type.
   * @param floating The floating-point value to initialize the Node with.
   */
  template <trait::Floating F>
  Node(F floating) : value_(Floating(floating)) {}

  /**
   * @brief Constructs a Node from a sequence of nodes.
   * @tparam S A type that satisfies the `trait::Sequence` concept.
   * @param sequence The sequence to initialize the Node with.
   */
  template <trait::Sequence S>
  Node(S sequence) : value_(std::move(sequence)) {}

  /**
   * @brief Constructs a Node from an object.
   * @tparam O A type that satisfies the `trait::Object` concept.
   * @param object The object to initialize the Node with.
   */
  template <trait::Object O>
  Node(O object) : value_(std::move(object)) {}

  /**
   * @brief Constructs a Node from a custom type using a converter.
   *
   * This constructor is enabled for any type `Input` that has a defined
   * `ToConfig` converter (either internal or external).
   *
   * @tparam Input The custom type to convert from.
   * @param input The input object to convert.
   */
  template <trait::HasToConfigConverter Input>
  Node(const Input &input) : Node() {
    trait::ConvertToConfig(input, *this);
  }

  /**
   * @brief Checks if the Node holds a null value.
   * @return True if the Node is null, false otherwise.
   */
  inline bool IsNull() const { return std::holds_alternative<Null>(value_); }

  /**
   * @brief Checks if the Node holds a string value.
   * @return True if the Node is a string, false otherwise.
   */
  inline bool IsString() const {
    return std::holds_alternative<String>(value_);
  }

  /**
   * @brief Checks if the Node holds a boolean value.
   * @return True if the Node is a boolean, false otherwise.
   */
  inline bool IsBoolean() const {
    return std::holds_alternative<Boolean>(value_);
  }

  /**
   * @brief Checks if the Node holds an integer value.
   * @return True if the Node is an integer, false otherwise.
   */
  inline bool IsInteger() const {
    return std::holds_alternative<Integer>(value_);
  }

  /**
   * @brief Checks if the Node holds a floating-point value.
   * @return True if the Node is a floating-point value, false otherwise.
   */
  inline bool IsFloating() const {
    return std::holds_alternative<Floating>(value_);
  }

  /**
   * @brief Checks if the Node holds a sequence value.
   * @return True if the Node is a sequence, false otherwise.
   */
  inline bool IsSequence() const {
    return std::holds_alternative<Sequence>(value_);
  }

  /**
   * @brief Checks if the Node holds an object value.
   * @return True if the Node is an object, false otherwise.
   */
  inline bool IsObject() const {
    return std::holds_alternative<Object>(value_);
  }

  /**
   * @brief Checks if the Node holds a value of the specified type.
   * @tparam T The type to check against.
   * @return True if the Node holds the specified type, false otherwise.
   */
  template <typename T>
  constexpr bool Is() const {
    if constexpr (trait::IsString<T>) {
      return IsString();
    }
    if constexpr (trait::IsBoolean<T>) {
      return IsBoolean();
    }
    if constexpr (trait::IsInteger<T>) {
      return IsInteger();
    }
    if constexpr (trait::IsFloating<T>) {
      return IsFloating();
    }
    if constexpr (trait::IsSequence<T>) {
      return IsSequence();
    }
    if constexpr (trait::IsObject<T>) {
      return IsObject();
    }
    return false;
  }

  /**
   * @brief Accesses the Node's value as a string.
   * @return Reference to the string value.
   * @throws NodeError if the Node is not a string.
   */
  String &AsString() { return As<String>(*this); }

  /**
   * @brief Accesses the Node's value as a boolean.
   * @return Reference to the boolean value.
   * @throws NodeError if the Node is not a boolean.
   */
  Boolean &AsBoolean() { return As<Boolean>(*this); }

  /**
   * @brief Accesses the Node's value as an integer.
   * @return Reference to the integer value.
   * @throws NodeError if the Node is not an integer.
   */
  Integer &AsInteger() { return As<Integer>(*this); }

  /**
   * @brief Accesses the Node's value as a floating-point number.
   * @return Reference to the floating-point value.
   * @throws NodeError if the Node is not a floating-point number.
   */
  Floating &AsFloating() { return As<Floating>(*this); }

  /**
   * @brief Accesses the Node's value as a sequence.
   * @return Reference to the sequence value.
   * @throws NodeError if the Node is not a sequence.
   */
  Sequence &AsSequence() { return As<Sequence>(*this); }

  /**
   * @brief Accesses the Node's value as an object.
   * @return Reference to the object value.
   * @throws NodeError if the Node is not an object.
   */
  Object &AsObject() { return As<Object>(*this); }

  /**
   * @brief Accesses the Node's value as a const string.
   * @return Const reference to the string value.
   * @throws NodeError if the Node is not a string.
   */
  const String &AsString() const { return As<String>(*this); }

  /**
   * @brief Accesses the Node's value as a const boolean.
   * @return Const reference to the boolean value.
   * @throws NodeError if the Node is not a boolean.
   */
  const Boolean &AsBoolean() const { return As<Boolean>(*this); }

  /**
   * @brief Accesses the Node's value as a const integer.
   * @return Const reference to the integer value.
   * @throws NodeError if the Node is not an integer.
   */
  const Integer &AsInteger() const { return As<Integer>(*this); }

  /**
   * @brief Accesses the Node's value as a const floating-point number.
   * @return Const reference to the floating-point value.
   * @throws NodeError if the Node is not a floating-point number.
   */
  const Floating &AsFloating() const { return As<Floating>(*this); }

  /**
   * @brief Accesses the Node's value as a const sequence.
   * @return Const reference to the sequence value.
   * @throws NodeError if the Node is not a sequence.
   */
  const Sequence &AsSequence() const { return As<Sequence>(*this); }

  /**
   * @brief Accesses the Node's value as a const object.
   * @return Const reference to the object value.
   * @throws NodeError if the Node is not an object.
   */
  const Object &AsObject() const { return As<Object>(*this); }

  /**
   * @brief Converts the Node to a string-like type.
   * @tparam S A type that satisfies the `trait::String` concept.
   * @return The string value.
   * @throws NodeError if the Node is not a String.
   */
  template <trait::String S>
  S To() const {
    return AsString();
  }
  /**
   * @brief Converts the Node to a boolean type.
   * @tparam B A type that satisfies the `trait::Boolean` concept.
   * @return The boolean value.
   * @throws NodeError if the Node is not a Boolean.
   */
  template <trait::Boolean B>
  B To() const {
    return AsBoolean();
  }
  /**
   * @brief Converts the Node to an integer type.
   * @tparam I A type that satisfies the `trait::Integer` concept.
   * @return The integer value.
   * @throws NodeError if the Node is not an Integer.
   */
  template <trait::Integer I>
  I To() const {
    return AsInteger();
  }
  /**
   * @brief Converts the Node to a floating-point type.
   * @tparam F A type that satisfies the `trait::Floating` concept.
   * @return The floating-point value.
   * @throws NodeError if the Node is not a Floating point.
   */
  template <trait::Floating F>
  F To() const {
    return AsFloating();
  }
  /**
   * @brief Converts the Node to a sequence type.
   * @tparam S A type that satisfies the `trait::Sequence` concept.
   * @return The sequence value.
   * @throws NodeError if the Node is not a Sequence.
   */
  template <trait::Sequence S>
  S To() const {
    return AsSequence();
  }
  /**
   * @brief Converts the Node to an object type.
   * @tparam O A type that satisfies the `trait::Object` concept.
   * @return The object value.
   * @throws NodeError if the Node is not an Object.
   */
  template <trait::Object O>
  O To() const {
    return AsObject();
  }
  /**
   * @brief Converts the Node to a custom user type.
   *
   * This conversion is enabled for any type `Output` that has a defined
   * `FromConfig` converter (either internal or external).
   *
   * @tparam Output The custom type to convert to.
   * @return An object of type `Output` initialized from the Node's data.
   */
  template <trait::HasFromConfigConverter Output>
  Output To() const {
    Output output;
    trait::ConvertFromConfig(*this, output);

    return output;
  }

  /**
   * @brief Conversion operator to String reference.
   * @return Reference to the string value.
   * @throws NodeError if the Node is not a string.
   */
  operator String &() { return AsString(); }

  /**
   * @brief Conversion operator to Boolean reference.
   * @return Reference to the boolean value.
   * @throws NodeError if the Node is not a boolean.
   */
  operator Boolean &() { return AsBoolean(); }

  /**
   * @brief Conversion operator to Integer reference.
   * @return Reference to the integer value.
   * @throws NodeError if the Node is not an integer.
   */
  operator Integer &() { return AsInteger(); }

  /**
   * @brief Conversion operator to Floating reference.
   * @return Reference to the floating-point value.
   * @throws NodeError if the Node is not a floating-point number.
   */
  operator Floating &() { return AsFloating(); }

  /**
   * @brief Conversion operator to Sequence reference.
   * @return Reference to the sequence value.
   * @throws NodeError if the Node is not a sequence.
   */
  operator Sequence &() { return AsSequence(); }

  /**
   * @brief Conversion operator to Object reference.
   * @return Reference to the object value.
   * @throws NodeError if the Node is not an object.
   */
  operator Object &() { return AsObject(); }

  /**
   * @brief Conversion operator to const String reference.
   * @return Const reference to the string value.
   * @throws NodeError if the Node is not a string.
   */
  operator const String &() const { return AsString(); }

  /**
   * @brief Conversion operator to const Boolean reference.
   * @return Const reference to the boolean value.
   * @throws NodeError if the Node is not a boolean.
   */
  operator const Boolean &() const { return AsBoolean(); }

  /**
   * @brief Conversion operator to const Integer reference.
   * @return Const reference to the integer value.
   * @throws NodeError if the Node is not an integer.
   */
  operator const Integer &() const { return AsInteger(); }

  /**
   * @brief Conversion operator to const Floating reference.
   * @return Const reference to the floating-point value.
   * @throws NodeError if the Node is not a floating-point number.
   */
  operator const Floating &() const { return AsFloating(); }

  /**
   * @brief Conversion operator to const Sequence reference.
   * @return Const reference to the sequence value.
   * @throws NodeError if the Node is not a sequence.
   */
  operator const Sequence &() const { return AsSequence(); }

  /**
   * @brief Conversion operator to const Object reference.
   * @return Const reference to the object value.
   * @throws NodeError if the Node is not an object.
   */
  operator const Object &() const { return AsObject(); }

  /**
   * @brief Conversion operator to an integral type.
   * @tparam I An integral type.
   * @return The node's value as an integer of type `I`.
   * @throws NodeError if the node is not an integer.
   */
  template <trait::Integer I>
  operator I() const {
    return To<I>();
  }

  /**
   * @brief Conversion operator to a floating-point type.
   * @tparam F A floating-point type.
   * @return The node's value as a floating-point number of type `F`.
   * @throws NodeError if the node is not a floating-point number.
   */
  template <trait::Floating F>
  operator F() const {
    return To<F>();
  }

  /**
   * @brief Conversion operator to a custom type with a `FromConfig` converter.
   * @tparam O The custom type to convert to.
   * @return The converted custom object.
   */
  template <trait::HasFromConfigConverter O>
  operator O() const {
    return To<O>();
  }

  /**
   * @brief Assigns a string value to the Node.
   * @tparam S A type satisfying `trait::String`.
   * @param string The string value to assign.
   * @return Reference to the Node.
   */
  template <trait::String S>
  Node &operator=(S string) {
    value_ = String(std::move(string));
    return *this;
  }

  /**
   * @brief Assigns a boolean value to the Node.
   * @param boolean The boolean value to assign.
   * @return Reference to the Node.
   */
  Node &operator=(Boolean boolean) {
    value_ = boolean;
    return *this;
  }

  /**
   * @brief Assigns an integer value to the Node.
   * @tparam I An integral type.
   * @param integer The integer value to assign.
   * @return Reference to the Node.
   */
  template <trait::Integer I>
  Node &operator=(I integer) {
    value_ = Integer(integer);
    return *this;
  }

  /**
   * @brief Assigns a floating-point value to the Node.
   * @tparam F A floating-point type.
   * @param floating The floating-point value to assign.
   * @return Reference to the Node.
   */
  template <trait::Floating F>
  Node &operator=(F floating) {
    value_ = Floating(floating);
    return *this;
  }

  /**
   * @brief Assigns a sequence value to the Node.
   * @tparam S A type satisfying `trait::Sequence`.
   * @param sequence The sequence to assign.
   * @return Reference to the Node.
   */
  template <trait::Sequence S>
  Node &operator=(S sequence) {
    value_ = std::move(sequence);
    return *this;
  }

  /**
   * @brief Assigns an object value to the Node.
   * @tparam O A type satisfying `trait::Object`.
   * @param object The object to assign.
   * @return Reference to the Node.
   */
  template <trait::Object O>
  Node &operator=(O object) {
    value_ = std::move(object);
    return *this;
  }

  /**
   * @brief Assigns from a custom type using a converter.
   *
   * This assignment operator is enabled for any type `Input` that has a defined
   * `ToConfig` converter (either internal or external).
   *
   * @tparam Input The custom type to assign from.
   * @param input The input object to convert and assign.
   * @return A reference to this Node.
   */
  template <trait::HasToConfigConverter Input>
  Node &operator=(const Input &input) {
    trait::ConvertToConfig(input, *this);
    return *this;
  }

  /**
   * @brief Accesses or creates a field in an object Node.
   * @param field The field name to access.
   * @return Reference to the Node associated with the field.
   * @throws NodeError if the Node is not an object or null.
   */
  Node &operator[](const char *field) {
    if (!IsObject() && !IsNull()) {
      throw NodeError(
          std::format("Cannot access field '{}' on non-object node.", field));
    }
    if (IsNull()) {
      value_ = Object();
    }
    return AsObject()[field];
  }

  /**
   * @brief Accesses or creates a field in an object Node.
   * @param field The field name to access (std::string).
   * @return Reference to the Node associated with the field.
   */
  Node &operator[](const std::string &field) {
    return operator[](field.data());
  }

  /**
   * @brief Accesses or creates a field in an object Node.
   * @param field The field name to access (std::string_view).
   * @return Reference to the Node associated with the field.
   */
  Node &operator[](std::string_view field) { return operator[](field.data()); }

  /**
   * @brief Accesses a field in a const object Node.
   * @param field The field name to access.
   * @return Const reference to the Node associated with the field.
   * @throws std::out_of_range if the field does not exist.
   */
  const Node &operator[](const char *field) const {
    return AsObject().at(field);
  }

  /**
   * @brief Accesses a field in a const object Node.
   * @param field The field name to access (std::string).
   * @return Const reference to the Node associated with the field.
   */
  const Node &operator[](const std::string &field) const {
    return operator[](field.data());
  }

  /**
   * @brief Accesses a field in a const object Node.
   * @param field The field name to access (std::string_view).
   * @return Const reference to the Node associated with the field.
   */
  const Node &operator[](std::string_view field) const {
    return operator[](field.data());
  }

  /**
   * @brief Returns the number of elements in the node.
   *
   * This function determines the size of the node based on its type. For
   * sequence nodes, it returns the number of elements in the sequence. For
   * object nodes, it returns the number of key-value pairs. For all other node
   * types, it returns zero.
   *
   * @return The number of elements in the node (sequence size, object size, or
   * 0 for other types).
   */
  std::uint64_t Length() const {
    if (IsSequence()) {
      return AsSequence().size();
    }
    if (IsObject()) {
      return AsObject().size();
    }
    return 0;
  }

  /**
   * @brief Accesses an element in a sequence node by index.
   *
   * This subscript operator allows non-const access to an element in a sequence
   * node. It delegates to the SubscriptOperator function to perform the access
   * and validation.
   *
   * @param index The index of the element to access.
   * @return A non-const reference to the node at the specified index.
   * @throws NodeError If the node is not a sequence or the index is out of
   * bounds.
   */
  Node &operator[](int index) { return SubscriptOperator(*this, index); }

  /**
   * @brief Accesses an element in a sequence node by index (const version).
   *
   * This subscript operator allows const access to an element in a sequence
   * node. It delegates to the SubscriptOperator function to perform the access
   * and validation.
   *
   * @param index The index of the element to access.
   * @return A const reference to the node at the specified index.
   * @throws NodeError If the node is not a sequence or the index is out of
   * bounds.
   */
  const Node &operator[](int index) const {
    return SubscriptOperator(*this, index);
  }

  /**
   * @brief Returns the Node's value if it is of a compatible type, otherwise
   * returns a default value.
   *
   * This function attempts to convert the Node to type `T`. If the Node holds a
   * value of a type that can be converted to `T` (e.g., Node holds an Integer
   * and `T` is `int`), it returns the converted value. Otherwise, it returns
   * the provided default value.
   *
   * @tparam T The type of the value to retrieve.
   * @param value The default value to return if the Node cannot be converted to
   * type `T`.
   * @return The Node's value as type T, or the default value.
   */
  template <typename T>
  T ValueOr(const T &value) {
    if constexpr (Is<T>()) {
      return To<T>();
    } else {
      return value;
    }
  }

  /**
   * @brief Checks if an object Node contains a specific field.
   * @param field The field name to check.
   * @return True if the field exists in an object Node, false otherwise.
   */
  bool Contains(const char *field) const {
    if (!IsObject()) {
      return false;
    }
    return AsObject().count(field) > 0;
  }

  /**
   * @brief Appends a node to a sequence node.
   *
   * This function adds a given node to the end of the current node, which must
   * be a sequence. If the current node is null, it is converted to an empty
   * sequence before appending. If the node is neither a sequence nor null, an
   * error is thrown.
   *
   * @param node The node to append to the sequence.
   * @throws NodeError If the current node is neither a sequence nor null.
   */
  void Push(const Node &node) {
    if (!IsSequence() && !IsNull()) {
      throw NodeError("Cannot push to non-sequence node.");
    }
    if (IsNull()) {
      value_ = Sequence();
    }
    AsSequence().push_back(node);
  }

  /**
   * @brief Determines the type of the node.
   *
   * This function checks the node's type by calling the appropriate
   * type-checking methods (e.g., IsNull, IsString, etc.) and returns the
   * corresponding NodeType enum value. If the node's type cannot be determined,
   * an exception is thrown.
   *
   * @return The NodeType enum value representing the node's type.
   * @throws NodeError If the node's type cannot be determined.
   */
  NodeType Type() const {
    if (IsNull()) {
      return NodeType::Null;
    }
    if (IsString()) {
      return NodeType::String;
    }
    if (IsBoolean()) {
      return NodeType::Boolean;
    }
    if (IsInteger()) {
      return NodeType::Integer;
    }
    if (IsFloating()) {
      return NodeType::Floating;
    }
    if (IsSequence()) {
      return NodeType::Sequence;
    }
    if (IsObject()) {
      return NodeType::Object;
    }
    throw NodeError("Failed to determine node type.");
  }

 private:
  /// The variant holding the Node's value.
  Value value_;

  /**
   * @brief Provides subscript operator access to a sequence node.
   *
   * @tparam Self The type of the node (const or non-const).
   * @param self Reference to the node being accessed.
   * @param index The index of the element to access.
   * @return A reference (const or non-const) to the node at the specified
   * index.
   * @throws NodeError If the node is not a sequence or the index is out of
   * bounds.
   *
   */
  template <typename Self>
  static std::conditional_t<std::is_const_v<Self>, const Node &, Node &>
  SubscriptOperator(Self &self, std::uint64_t index) {
    if (!self.IsSequence()) {
      throw NodeError("Cannot access node. Node is not a sequence");
    }
    if (index >= self.Length()) {
      throw NodeError(
          std::format("Index out of bounds\n"
                      "Index  : {}        \n"
                      "Length : {}",

                      index, self.Length()));
    }
    return self.AsSequence()[index];
  }

  /**
   * @brief Demangles a type name for error reporting.
   * @tparam T The type to demangle.
   * @return The demangled type name as a string.
   */
  template <typename T>
  static std::string Demangle() {
#ifndef __GNUC__
    return typeid(T).name();
#else
    int status;
    std::unique_ptr<char, decltype(&free)> demangled(
        abi::__cxa_demangle(typeid(T).name(), 0, 0, &status), free);
    return status == 0 ? std::string(demangled.get())
                       : std::string(typeid(T).name());
#endif
  }

  /**
   * @brief Internal helper to access the Node's value as a specific type.
   * @tparam T The target type.
   * @tparam Self The type of the Node (const or non-const).
   * @param self Reference to the Node.
   * @return Reference to the value as type T.
   * @throws NodeError if the Node does not hold type T.
   */
  template <typename T, typename Self>
  static std::conditional_t<std::is_const_v<Self>, const T &, T &> As(
      Self &self) {
    try {
      return std::get<T>(self.value_);
    } catch (...) {
      throw NodeError(std::format("Conversion {} -> {} failed.",
                                  self.TypeString(), Demangle<T>()));
    }
  }

  /**
   * @brief Returns a string representation of the Node's type.
   * @return The type name as a string (e.g., "Null", "String", etc.).
   */
  std::string TypeString() const noexcept {
    if (IsNull()) {
      return "Null";
    }
    if (IsString()) {
      return "String";
    }
    if (IsBoolean()) {
      return "Boolean";
    }
    if (IsInteger()) {
      return "Integer";
    }
    if (IsFloating()) {
      return "Floating";
    }
    if (IsSequence()) {
      return "Sequence";
    }
    if (IsObject()) {
      return "Object";
    }
    return "Undefined";
  }
};
};  // namespace Config
