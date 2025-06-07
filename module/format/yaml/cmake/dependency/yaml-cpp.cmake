
include(FetchContent) 

message(STATUS "Downoading jbeder/yaml-cpp...")

FetchContent_Declare(
  yaml-cpp 
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git 
  GIT_TAG        2f86d13775d119edbb69af52e5f566fd65c6953b
)
FetchContent_MakeAvailable(
  yaml-cpp
)
add_library(dependency::yaml-cpp ALIAS yaml-cpp)

target_compile_features(yaml-cpp PUBLIC cxx_std_17)
