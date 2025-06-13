
cmake_minimum_required(VERSION 3.23)

project(config LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
  dependency-cmake 
  GIT_REPOSITORY https://github.com/weqeqq/dependency-cmake
  GIT_TAG        1d103e5cff768665a750915500b7dc826220333c
)
FetchContent_MakeAvailable(dependency-cmake)

include(GenerateExportHeader)

function(GenerateExportHeader target file)
  set(export_header_dir 
    ${CMAKE_BINARY_DIR}/export-header PARENT_SCOPE)

  set(export_header_file 
    ${CMAKE_BINARY_DIR}/export-header/${file})

  generate_export_header(
    ${target} 
    EXPORT_FILE_NAME 
    ${CMAKE_BINARY_DIR}/export-header/${file}
  ) 
endfunction()

add_subdirectory(module/core)
add_subdirectory(module/format)
add_subdirectory(module/io)

add_subdirectory(lib)
