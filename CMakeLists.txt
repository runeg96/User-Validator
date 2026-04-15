cmake_minimum_required(VERSION 3.12)

project(JsonParser VERSION 1.0 LANGUAGES CXX)

include(GNUInstallDirs)

# Default to Release only for single-config generators.
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Development convenience: copy runtime files into the build directory.
configure_file(
    ${CMAKE_SOURCE_DIR}/users.json
    ${CMAKE_BINARY_DIR}/users.json
    COPYONLY
)

# Build pugixml as a separate target.
add_library(pugixml STATIC
    external/pugixml/src/pugixml.cpp
)

target_include_directories(pugixml
    PUBLIC
        ${CMAKE_SOURCE_DIR}/external/pugixml/src
)

add_executable(json_parser
    src/parser/parser_common.cpp
    src/parser/user_parser.cpp
    src/utils/utils.cpp
    src/validator/user_rules.cpp
    src/validator/user_validator.cpp
    src/mapper/user_json_mapper.cpp
    src/exporter/json_exporter.cpp
    src/exporter/xml_exporter.cpp
    src/exporter/exporter_factory.cpp
    src/exporter/export_service.cpp
    src/logging/logger.cpp
    src/main.cpp
)

target_include_directories(json_parser
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/external/rapidjson/include
)

target_compile_features(json_parser PRIVATE cxx_std_17)
set_target_properties(json_parser PROPERTIES CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(json_parser PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_link_libraries(json_parser PRIVATE pugixml)

install(TARGETS json_parser
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)