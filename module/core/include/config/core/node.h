/**
 * @file node.h
 * @brief Defines the Node class for handling configuration data with variant types.
 *
 * This header provides the Node class, which is used to represent configuration data
 * in a type-safe manner using std::variant. It supports various data types such as
 * null, string, boolean, integer, floating-point, sequence, and object. The class
 * includes methods for type checking, conversion, and accessing nested fields.
 */

#pragma once

#include <type_traits>
#include <variant>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

#include <config/core/error.h>

#ifdef __GNUC__
#include <cxxabi.h>
#endif

namespace Config {

class Node;

template <typename T>
struct Serialiser {

  Serialiser(const T &, Node &) {
    static_assert(false, "Unimplemented");
  }
};

template <typename T>
struct Deserialiser {

  Deserialiser(const Node &, T &) {
    static_assert(false, "Unimplemented");
  }
};

namespace Detail {

template <typename Serialiser, typename Input>
concept SerialiserImpl = 
requires(
  const Input &input, Node &node
) { { Serialiser(input, node)() } -> std::same_as<void>; };

template <typename Input>
concept HasExternalSerialiserImpl = 
  SerialiserImpl<Serialiser<Input>, Input>;

template <typename Input>
concept HasInternalSerialiserImpl = 
  SerialiserImpl<typename Input::Serialiser, Input>;

template <typename Input>
concept HasSerialiserImpl = 
  HasExternalSerialiserImpl<Input> || 
  HasInternalSerialiserImpl<Input>;

namespace {
template <HasExternalSerialiserImpl Input>
void CallSerialiser(const Input &input, Node &node) {
  Serialiser<Input>(input, node)();
}
template <HasInternalSerialiserImpl Input>
void CallSerialiser(const Input &input, Node &node) {
  typename Input::Serialiser(input, node)();
}
}
template <typename Deserialiser, typename Output>
concept DeserialiserImpl = 
requires(
  const Node &node, Output &output 
) { { Deserialiser(node, output)() } -> std::same_as<void>; };

template <typename Output> 
concept HasExternalDeserialiserImpl = 
  DeserialiserImpl<Deserialiser<Output>, Output>;

template <typename Output>
concept HasInternalDeserialiserImpl = 
  DeserialiserImpl<typename Output::Deserialiser, Output>;

template <typename Output>
concept HasDeserialiserImpl = 
  HasExternalDeserialiserImpl<Output> || 
  HasInternalDeserialiserImpl<Output>;

namespace {
template <HasExternalDeserialiserImpl Output>
void CallDeserialiser(const Node &node, Output &output) {
  Deserialiser<Output>(node, output)();
}
template <HasInternalDeserialiserImpl Output>
void CallDeserialiser(const Node &node, Output &output) {
  typename Output::Deserialiser(node, output)();
}
}
}

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
  NodeError(std::string_view message) : Error(Message("NodeError", message)) {}
};

/**
 * @brief Enumerates the possible types of a node in a data structure.
 *
 * This enum class defines the different types a node can have, such as null, string,
 * boolean, integer, floating-point, sequence, or object. It is used to identify the
 * type of data stored in a node.
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
 * The Node class encapsulates a std::variant that can hold different types of data:
 * null, string, boolean, integer, floating-point, sequence, or object. It provides
 * methods for type checking, conversion, and accessing nested data.
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
  /// Type alias for object mapping strings to nodes (std::unordered_map<std::string, Node>).
  using Object = std::unordered_map<std::string, Node>;

  /// Variant type that can hold any of the supported data types.
  using Value = std::variant<Null, String, Boolean, Integer, Floating, Sequence, Object>;

  /**
   * @brief Default constructor initializing the Node to Null.
   */
  Node() : value_(Null()) {}

  /**
   * @brief Constructs a Node from a string.
   * @param string The string value to initialize the Node with.
   */
  Node(String string) : value_(std::move(string)) {}

  /**
   * @brief Constructs a Node from a C-string.
   * @param c_string The C-string to initialize the Node with.
   */
  Node(const char *c_string) : value_(String(c_string)) {}

  /**
   * @brief Constructs a Node from a string view.
   * @param string_view The string view to initialize the Node with.
   */
  Node(std::string_view string_view) : value_(String(string_view)) {}

  /**
   * @brief Constructs a Node from a boolean.
   * @param boolean The boolean value to initialize the Node with.
   */
  Node(Boolean boolean) : value_(boolean) {}

  /**
   * @brief Constructs a Node from an integer.
   * @param integer The integer value to initialize the Node with.
   */
  Node(Integer integer) : value_(integer) {}

  /**
   * @brief Constructs a Node from a floating-point value.
   * @param floating The floating-point value to initialize the Node with.
   */
  Node(Floating floating) : value_(floating) {}

  /**
   * @brief Constructs a Node from a sequence of nodes.
   * @param sequence The sequence to initialize the Node with.
   */
  Node(Sequence sequence) : value_(std::move(sequence)) {}

  /**
   * @brief Constructs a Node from an object.
   * @param object The object to initialize the Node with.
   */
  Node(Object object) : value_(std::move(object)) {}

  template <Detail::HasSerialiserImpl Input>
  Node(Input input) : Node() {
    Detail::CallSerialiser(input, *this);
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
  inline bool IsString() const { return std::holds_alternative<String>(value_); }

  /**
   * @brief Checks if the Node holds a boolean value.
   * @return True if the Node is a boolean, false otherwise.
   */
  inline bool IsBoolean() const { return std::holds_alternative<Boolean>(value_); }

  /**
   * @brief Checks if the Node holds an integer value.
   * @return True if the Node is an integer, false otherwise.
   */
  inline bool IsInteger() const { return std::holds_alternative<Integer>(value_); }

  /**
   * @brief Checks if the Node holds a floating-point value.
   * @return True if the Node is a floating-point value, false otherwise.
   */
  inline bool IsFloating() const { return std::holds_alternative<Floating>(value_); }

  /**
   * @brief Checks if the Node holds a sequence value.
   * @return True if the Node is a sequence, false otherwise.
   */
  inline bool IsSequence() const { return std::holds_alternative<Sequence>(value_); }

  /**
   * @brief Checks if the Node holds an object value.
   * @return True if the Node is an object, false otherwise.
   */
  inline bool IsObject() const { return std::holds_alternative<Object>(value_); }

  /**
   * @brief Checks if the Node holds a value of the specified type.
   * @tparam T The type to check against.
   * @return True if the Node holds the specified type, false otherwise.
   */
  template <typename T>
  constexpr bool Is() const {
    if constexpr (IsStringT<T>) {
      return IsString();
    }
    if constexpr (IsBooleanT<T>) {
      return IsBoolean();
    }
    if constexpr (IsIntegerT<T>) {
      return IsInteger();
    }
    if constexpr (IsFloatingT<T>) {
      return IsFloating();
    }
    if constexpr (IsSequenceT<T>) {
      return IsSequence();
    }
    if constexpr (IsObjectT<T>) {
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
   * @brief Converts the Node's value to the specified type.
   * @tparam T The target type for conversion.
   * @return The converted value.
   * @throws NodeError if conversion is not possible.
   */
  template <typename T>
  T To() {
    if constexpr (IsStringT<T>) {
      return static_cast<T>(AsString());
    }
    if constexpr (IsBooleanT<T>) {
      return static_cast<T>(AsBoolean());
    }
    if constexpr (IsIntegerT<T>) {
      return static_cast<T>(AsInteger());
    }
    if constexpr (IsFloatingT<T>) {
      return static_cast<T>(AsFloating());
    }
    if constexpr (IsSequenceT<T>) {
      return static_cast<T>(AsSequence());
    }
    if constexpr (IsObjectT<T>) {
      return static_cast<T>(AsObject());
    }
    throw NodeError(std::format(
        "Conversion {} -> {} failed.", TypeString(), Demangle<T>()));
  }
  template <Detail::HasDeserialiserImpl Output>
  Output To() {
    Output output;
    Detail::CallDeserialiser(*this, output);

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
   * @brief Assigns a string value to the Node.
   * @param string The string value to assign.
   * @return Reference to the Node.
   */
  Node &operator=(String string) {
    value_ = std::move(string);
    return *this;
  }

  /**
   * @brief Assigns a string view to the Node.
   * @param string_view The string view to assign.
   * @return Reference to the Node.
   */
  Node &operator=(std::string_view string_view) {
    return operator=(String(string_view));
  }

  /**
   * @brief Assigns a C-string to the Node.
   * @param c_string The C-string to assign.
   * @return Reference to the Node.
   */
  Node &operator=(const char *c_string) {
    return operator=(String(c_string));
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
   * @param integer The integer value to assign.
   * @return Reference to the Node.
   */
  Node &operator=(Integer integer) {
    value_ = integer;
    return *this;
  }

  /**
   * @brief Assigns a floating-point value to the Node.
   * @param floating The floating-point value to assign.
   * @return Reference to the Node.
   */
  Node &operator=(Floating floating) {
    value_ = floating;
    return *this;
  }

  /**
   * @brief Assigns a sequence value to the Node.
   * @param sequence The sequence to assign.
   * @return Reference to the Node.
   */
  Node &operator=(Sequence sequence) {
    value_ = sequence;
    return *this;
  }

  /**
   * @brief Assigns an object value to the Node.
   * @param object The object to assign.
   * @return Reference to the Node.
   */
  Node &operator=(Object object) {
    value_ = object;
    return *this;
  }

  template <Detail::HasSerialiserImpl Input>
  Node &operator=(Input input) {
    Detail::CallSerialiser(input, *this);
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
  Node &operator[](const std::string &field) { return operator[](field.data()); }

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
  const Node &operator[](const char *field) const { return AsObject().at(field); }

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
   * This function determines the size of the node based on its type. For sequence nodes,
   * it returns the number of elements in the sequence. For object nodes, it returns the
   * number of key-value pairs. For all other node types, it returns zero.
   *
   * @return The number of elements in the node (sequence size, object size, or 0 for other types).
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
   * This subscript operator allows non-const access to an element in a sequence node.
   * It delegates to the SubscriptOperator function to perform the access and validation.
   *
   * @param index The index of the element to access.
   * @return A non-const reference to the node at the specified index.
   * @throws NodeError If the node is not a sequence or the index is out of bounds.
   */
  Node &operator[](int index) {
    return SubscriptOperator(*this, index);
  }

  /**
   * @brief Accesses an element in a sequence node by index (const version).
   *
   * This subscript operator allows const access to an element in a sequence node.
   * It delegates to the SubscriptOperator function to perform the access and validation.
   *
   * @param index The index of the element to access.
   * @return A const reference to the node at the specified index.
   * @throws NodeError If the node is not a sequence or the index is out of bounds.
   */
  const Node &operator[](int index) const {
    return SubscriptOperator(*this, index);
  }

  /**
   * @brief Returns the Node's value or a default value if the type does not match.
   * @tparam T The type of the value to retrieve.
   * @param value The default value to return if the type does not match.
   * @return The Node's value if it matches type T, otherwise the default value.
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
   * This function adds a given node to the end of the current node, which must be a sequence.
   * If the current node is null, it is converted to an empty sequence before appending.
   * If the node is neither a sequence nor null, an error is thrown.
   *
   * @param node The node to append to the sequence.
   * @throws NodeError If the current node is neither a sequence nor null.
   */
  void Push(const Node &node) {
    if (!IsSequence() && !IsNull()) {
      throw NodeError(
          "Cannot push to non-sequence node.");
    }
    if (IsNull()) {
      value_ = Sequence();
    }
    AsSequence().push_back(node);
  }

  /**
   * @brief Determines the type of the node.
   *
   * This function checks the node's type by calling the appropriate type-checking methods
   * (e.g., IsNull, IsString, etc.) and returns the corresponding NodeType enum value.
   * If the node's type cannot be determined, an exception is thrown.
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

  /// @cond INTERNAL
  /// Type trait to check if a type is String.
  template <typename T>
  static constexpr bool IsStringT = std::is_same_v<T, String>;

  /// Type trait to check if a type is Boolean.
  template <typename T>
  static constexpr bool IsBooleanT = std::is_same_v<T, Boolean>;

  /// Type trait to check if a type is an integral type.
  template <typename T>
  static constexpr bool IsIntegerT = std::is_integral_v<T>;

  /// Type trait to check if a type is a floating-point type.
  template <typename T>
  static constexpr bool IsFloatingT = std::is_floating_point_v<T>;

  /// Type trait to check if a type is Sequence.
  template <typename T>
  static constexpr bool IsSequenceT = std::is_same_v<T, Sequence>;

  /// Type trait to check if a type is Object.
  template <typename T>
  static constexpr bool IsObjectT = std::is_same_v<T, Object>;
  /// @endcond

  /**
   * @brief Provides subscript operator access to a sequence node. 
   *
   * @tparam Self The type of the node (const or non-const).
   * @param self Reference to the node being accessed. 
   * @param index The index of the element to access. 
   * @return A reference (const or non-const) to the node at the specified index. 
   * @throws NodeError If the node is not a sequence ifor the index is out of bounds.
   *
   */
  template <typename Self>
  static std::conditional_t<std::is_const_v<Self>,
    const Node&,
          Node&> 
  SubscriptOperator(Self &self, std::uint64_t index) {
    if (!self.IsSequence()) {
      throw NodeError(
        "Cannot access node. Node is not a sequence"
      );
    } 
    if (index >= self.Length()) {
      throw NodeError(
        std::format(
          "Index out of bounds\n"
          "Index  : {}        \n"
          "Length : {}", 

          index, self.Length()
        )
      );
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
  static std::conditional_t<std::is_const_v<Self>, const T &, T &> As(Self &self) {
    try {
      return std::get<T>(self.value_);
    } catch (...) {
      throw NodeError(std::format(
          "Conversion {} -> {} failed.", self.TypeString(), Demangle<T>()));
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
};
