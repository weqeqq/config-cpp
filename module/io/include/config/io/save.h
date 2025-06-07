
#pragma once 

#include <config/io/error.h>
#include <config/core/node.h>
#include <filesystem>
#include <config/io/export.h>

namespace Config {

struct SaveError : IoError {
  SaveError(std::string_view message) 
    : IoError(Message("SaveError", message)) {}
};

CONFIG_IO_EXPORT;
void Save(
  const Node &node, 
  const std::filesystem::path &path
);

}
