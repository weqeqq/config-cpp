
#include <config/io/open.h>
#include <config/format/json.h>
#include <config/format/yaml.h>

#include <file/read_string.h>

namespace Config {

Node Open(const std::filesystem::path &path) {
  auto extension = path.extension();

  if (extension == ".json") {
    return JsonFormat::Parse(File::ReadString(path));
  }
  if (extension == ".yml" || 
      extension == ".yaml") {
    return YamlFormat::Parse(File::ReadString(path));
  }
  throw OpenError("Undefined extension");
}
}
