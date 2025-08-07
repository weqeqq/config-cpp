
cmake_minimum_required(VERSION 3.23)

project(ConfigCpp LANGUAGES CXX)

option(CONFIG_CPP_BUILD_SHARED "" ON)
option(CONFIG_CPP_BUILD_STATIC "" ON)

option(CONFIG_CPP_YAML_SUPPORT "" ON)
option(CONFIG_CPP_JSON_SUPPORT "" ON)

option(CONFIG_CPP_BUILD_TESTS "" ${PROJECT_IS_TOP_LEVEL})
option(CONFIG_CPP_BUILD_DOCS  "" ${PROJECT_IS_TOP_LEVEL})
option(CONFIG_CPP_FETCH_DEPS  "" ON)

if(NOT CONFIG_CPP_FETCH_DEPS)
    find_package(FileCpp CONFIG REQUIRED)
    if(CONFIG_CPP_JSON_SUPPORT)
        find_package(nlohmann_json CONFIG REQUIRED)
    endif()
    if(CONFIG_CPP_YAML_SUPPORT)
        find_package(yaml-cpp CONFIG REQUIRED)
    endif()
else()
    set(BUILD_SHARED_LIBS OFF)
    include(FetchContent)
    FetchContent_Declare(
        file-cpp
        GIT_REPOSITORY https://github.com/weqeqq/file-cpp.git
        GIT_TAG 65585cb0a504b252e30e27d3e60ef3ea4311095e
    )
    set(FILE_CPP_BUILD_TESTS OFF)
    set(FILE_CPP_BUILD_DOCS  OFF)
    FetchContent_MakeAvailable(file-cpp)
    if(CONFIG_CPP_YAML_SUPPORT)
        FetchContent_Declare(
            yaml-cpp
            GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
            GIT_TAG 2f86d13775d119edbb69af52e5f566fd65c6953b
        )
        set(YAML_CPP_BUILD_CONTRIB OFF)
        set(YAML_CPP_BUILD_TOOLS   OFF)
        set(YAML_CPP_BUILD_TESTS   OFF)
        FetchContent_MakeAvailable(yaml-cpp)
    endif()
    if(CONFIG_CPP_JSON_SUPPORT)
        FetchContent_Declare(
            json
            GIT_REPOSITORY https://github.com/nlohmann/json.git
            GIT_TAG e00484f8668dda6cfaf9887ec141c27c341e7de5
        )
        set(JSON_BuildTests OFF)
        FetchContent_MakeAvailable(json)
    endif()
endif()

include(GenerateExportHeader)
if(CONFIG_CPP_BUILD_SHARED)
    add_library(ConfigCppShared SHARED)
    add_library(ConfigCpp::Shared ALIAS ConfigCppShared)
    set(CONFIG_CPP_SHARED_EXPORT_DIR ${CMAKE_CURRENT_BINARY_DIR}/export/shared)
    set(CONFIG_CPP_SHARED_EXPORT_FILE ${CONFIG_CPP_SHARED_EXPORT_DIR}/config-cpp/export.h)
    generate_export_header(ConfigCppShared
        BASE_NAME          CONFIG_CPP
        EXPORT_FILE_NAME ${CONFIG_CPP_SHARED_EXPORT_FILE}
    )
endif()
if(CONFIG_CPP_BUILD_STATIC)
    add_library(ConfigCppStatic STATIC)
    add_library(ConfigCpp::Static ALIAS ConfigCppStatic)
    set(CONFIG_CPP_STATIC_EXPORT_DIR ${CMAKE_CURRENT_BINARY_DIR}/export/static)
    set(CONFIG_CPP_STATIC_EXPORT_FILE ${CONFIG_CPP_STATIC_EXPORT_DIR}/config-cpp/export.h)
    generate_export_header(ConfigCppShared
        BASE_NAME          CONFIG_CPP
        EXPORT_FILE_NAME ${CONFIG_CPP_STATIC_EXPORT_FILE}
    )
endif()
foreach(target ConfigCppShared ConfigCppStatic)
    if(TARGET ${target})
        target_compile_features(${target} PUBLIC cxx_std_20)
        target_sources(${target}
            PUBLIC
                FILE_SET HEADERS BASE_DIRS
                    headers
                    $<$<STREQUAL:${target},ConfigCppShared>:${CONFIG_CPP_SHARED_EXPORT_DIR}>
                    $<$<STREQUAL:${target},ConfigCppStatic>:${CONFIG_CPP_STATIC_EXPORT_DIR}>
                FILES
                    $<$<STREQUAL:${target},ConfigCppShared>:${CONFIG_CPP_SHARED_EXPORT_FILE}>
                    $<$<STREQUAL:${target},ConfigCppStatic>:${CONFIG_CPP_STATIC_EXPORT_FILE}>
                    headers/config-cpp/config.h
            PRIVATE
                sources/format/json/dump.cc
                sources/format/json/parse.cc
                sources/format/yaml/dump.cc
                sources/format/yaml/parse.cc
        )
        target_link_libraries(${target}
            PRIVATE
                nlohmann_json::nlohmann_json
                yaml-cpp::yaml-cpp
            PUBLIC
                FileCpp::FileCpp
        )
    endif()
endforeach()
if(CONFIG_CPP_BUILD_DOCS)
    find_package(Doxygen REQUIRED)
    include(FetchContent)
    FetchContent_Declare(
        doxygen-awesome-css
        GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css
        GIT_TAG 9760c30014131f4eacb8e96f15f3869c7bc5dd8c
    )
    FetchContent_MakeAvailable(doxygen-awesome-css)
    FetchContent_GetProperties(doxygen-awesome-css SOURCE_DIR AWESOME_CSS_DIR)

    set(DOXYGEN_GENERATE_TREEVIEW YES)
    set(DOXYGEN_DISABLE_INDEX     NO)
    set(DOXYGEN_FULL_SIDEBAR      NO)
    set(DOXYGEN_HTML_COLORSTYLE   LIGHT)
    set(DOXYGEN_HTML_EXTRA_STYLESHEET ${AWESOME_CSS_DIR}/doxygen-awesome.css)
    doxygen_add_docs(ConfigCppDocs
        ${CMAKE_CURRENT_SOURCE_DIR}/headers/config-cpp ALL
    )
endif()
