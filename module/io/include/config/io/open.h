
#pragma once 

#include <config/core/node.h>

#include <config/io/export.h>
#include <config/io/error.h>
#include <filesystem>

namespace Config {

struct OpenError : IoError {
  OpenError(std::string_view message) 
    : IoError(Message("OpenError", message)) {}
};

CONFIG_IO_EXPORT;
Node Open(const std::filesystem::path &path);

}
