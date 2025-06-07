
set(name config-format-yaml)

add_library(${name})
add_library(config::format::yaml ALIAS ${name})

include(cmake/generate-export-header.cmake)
target_sources(${name}
  PUBLIC 
    FILE_SET  HEADERS 
    BASE_DIRS include ${EXPORT_HEADER_BASE_DIR}
    FILES 
      include/config/format/yaml/dump.h
      include/config/format/yaml/error.h 
      include/config/format/yaml/parse.h 
      include/config/format/yaml.h

      ${EXPORT_HEADER_FILE_NAME}

  PRIVATE 
    source/dump.cc 
    source/parse.cc
)
include(cmake/dependency/yaml-cpp.cmake)

target_link_libraries(${name} 
  PUBLIC
    config::core 
  PRIVATE 
    dependency::yaml-cpp
)
