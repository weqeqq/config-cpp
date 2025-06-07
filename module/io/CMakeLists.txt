
set(name config-io)

add_library(${name})
add_library(config::io ALIAS ${name})

include(cmake/generate-export-header.cmake)
target_sources(${name}
  PUBLIC 
    FILE_SET  HEADERS 
    BASE_DIRS include ${EXPORT_HEADER_BASE_DIR}
    FILES 
      include/config/io/error.h 
      include/config/io/open.h
      include/config/io/save.h

      ${EXPORT_HEADER_FILE_NAME}

  PRIVATE 
    source/open.cc 
    source/save.cc 
)
include(cmake/dependency/file.cmake)

target_link_libraries(${name} 
  PUBLIC 
    config::core 
  PRIVATE 
    config::format
    dependency::file
)
