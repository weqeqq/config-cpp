
include(FetchContent)

message(STATUS "Downloading weqeqq/file-cpp...")

FetchContent_Declare(
  file 
  GIT_REPOSITORY https://github.com/weqeqq/file-cpp.git 
  GIT_TAG        06108062077d0d6abd8550988fd8cf5a9f5d3d11
  GIT_SHALLOW    ON
)
FetchContent_MakeAvailable(
  file
)
add_library(dependency::file ALIAS file)
