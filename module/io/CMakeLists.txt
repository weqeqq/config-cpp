
set(name config-io)

add_library(${name})
add_library(config::io ALIAS ${name})

GenerateExportHeader(
  ${name}
  config/io/export.h
)
target_sources(${name}
  PUBLIC 
    FILE_SET HEADERS BASE_DIRS 
      include 
      ${export_header_dir}
    FILES 
      include/config/io/error.h 
      include/config/io/open.h
      include/config/io/save.h
      ${export_header_file}
  PRIVATE 
    source/open.cc 
    source/save.cc 
)
Dependency(config::external::file
  REPOSITORY 
    weqeqq/file-cpp 
  TAG        
    d7eda176f32ec5b45eac172e2cddb08d562d3ebb
)
target_link_libraries(${name} 
  PUBLIC 
    config::core 
  PRIVATE 
    config::format
    config::external::file
)
