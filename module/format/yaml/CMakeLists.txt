
set(name config-format-yaml)

add_library(${name})
add_library(config::format::yaml ALIAS ${name})

GenerateExportHeader(
  ${name}
  config/format/yaml/export.h
)
target_sources(${name}
  PUBLIC 
    FILE_SET  HEADERS 
    BASE_DIRS include ${export_header_dir}
    FILES 
      include/config/format/yaml/dump.h
      include/config/format/yaml/error.h 
      include/config/format/yaml/parse.h 
      include/config/format/yaml.h
      ${export_header_file}

  PRIVATE 
    source/dump.cc 
    source/parse.cc
)
Dependency(config::external::yaml-cpp
  REPOSITORY 
    jbeder/yaml-cpp
  TAG 
    2f86d13775d119edbb69af52e5f566fd65c6953b
  EXPECTED 
    lib/libyaml-cpp.a
)
target_link_libraries(${name} 
  PUBLIC
    config::core 
  PRIVATE 
    config::external::yaml-cpp
)
