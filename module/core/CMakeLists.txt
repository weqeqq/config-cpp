
add_library(config__core INTERFACE)
add_library(config::core ALIAS config__core)

target_compile_features(config__core INTERFACE cxx_std_23)

set_target_properties(config__core PROPERTIES

  OUTPUT_NAME configcore
)
target_sources(config__core 
  PUBLIC 
    FILE_SET  HEADERS 
    BASE_DIRS include
    FILES 
      include/config/core/error.h
      include/config/core/node.h
)
