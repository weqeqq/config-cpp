
#pragma once 

#include <config/core/error.h>

namespace Config {

class IoError : public Error {
 public:
  IoError(std::string_view message) 
    : Error(Namespace("IoError", message)) {}
};

}
