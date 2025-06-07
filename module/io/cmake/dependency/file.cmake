
include(FetchContent)

message(STATUS "Downloading weqeqq/file-cpp...")

FetchContent_Declare(
  file 
  GIT_REPOSITORY https://github.com/weqeqq/file-cpp.git 
  GIT_TAG        309d31c128da07bcccf8fb5a67e2df52533d4d43
  GIT_SHALLOW    ON
)
FetchContent_MakeAvailable(
  file
)
add_library(dependency::file ALIAS file)
