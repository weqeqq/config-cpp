
include(FetchContent)

message(STATUS "Downoading nlohmann/json...")

FetchContent_Declare(
  nlohmann-json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        c633693d3e2ab81fa186c691c452c47ced107845
  GIT_SHALLOW    ON
)
FetchContent_MakeAvailable(
  nlohmann-json
)
add_library(dependency::nlohmann-json ALIAS nlohmann_json)
