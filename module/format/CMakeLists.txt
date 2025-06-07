
set(name config-format)

add_library(${name} INTERFACE)
add_library(config::format ALIAS ${name})

add_subdirectory(json)
add_subdirectory(yaml)

target_link_libraries(${name} INTERFACE 
  config::format::json
  config::format::yaml
)
