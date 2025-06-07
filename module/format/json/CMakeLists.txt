
set(name config-format-json)

add_library(${name})
add_library(config::format::json ALIAS ${name})

include(cmake/generate-export-header.cmake)
target_sources(${name}
  PUBLIC 
    FILE_SET  HEADERS 
    BASE_DIRS include ${EXPORT_HEADER_BASE_DIR}
    FILES 
      include/config/format/json/dump.h
      include/config/format/json/error.h
      include/config/format/json/parse.h
      include/config/format/json.h

      ${EXPORT_HEADER_FILE_NAME}
  PRIVATE 
    source/dump.cc
    source/parse.cc
)
include(cmake/dependency/nlohmann-json.cmake)

target_link_libraries(${name} 
  PUBLIC
    config::core
  PRIVATE
    dependency::nlohmann-json
)
