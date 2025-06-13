
set(name config-format-json)

add_library(${name})
add_library(config::format::json ALIAS ${name})

GenerateExportHeader(
  ${name}
  config/format/json/export.h
)
target_sources(${name}
  PUBLIC 
    FILE_SET HEADERS BASE_DIRS 
      include 
      ${export_header_dir}
    FILES 
      include/config/format/json/dump.h
      include/config/format/json/error.h
      include/config/format/json/parse.h
      include/config/format/json.h
      ${export_header_file}
  PRIVATE 
    source/dump.cc
    source/parse.cc
)
Dependency(config::external::nlohmann-json
  REPOSITORY
    nlohmann/json
  TAG 
    c633693d3e2ab81fa186c691c452c47ced107845
  DEFINE 
    JSON_BuildTests OFF 
)
target_link_libraries(${name} 
  PUBLIC
    config::core
  PRIVATE
    config::external::nlohmann-json
)
