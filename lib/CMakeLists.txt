
set(name config-lib)

add_library(${name} INTERFACE)
add_library(config::config ALIAS ${name})

target_sources(${name}
  PUBLIC 
    FILE_SET  HEADERS
    BASE_DIRS include 
    FILES 
      include/config/config.h
)
target_link_libraries(${name} INTERFACE
  config::core 
  config::io
)


