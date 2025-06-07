
#include <config/io/save.h>
#include <file/write_string.h>

#include <config/format/json.h>
#include <config/format/yaml.h>

namespace Config {
  
void Save(
  const Node &node, 
  const std::filesystem::path &path
) {
  auto extension = path.extension();

  if (extension == ".json") {
    return File::WriteString(path, JsonFormat::Dump(node));
  }
  if (extension == ".yml" || 
      extension == ".yaml") {
    return File::WriteString(path, YamlFormat::Dump(node));
  }
  throw SaveError("Undefined file format");
}

}
