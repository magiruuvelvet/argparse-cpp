cmake_minimum_required(VERSION 3.14)
project(libargparse CXX)

# create library and interface target
add_library(libargparse STATIC)
add_library(libargparse_iface INTERFACE)
add_library(libs::argparse ALIAS libargparse_iface)

# set target properties
set_target_properties(libargparse PROPERTIES PREFIX "")
set_target_properties(libargparse PROPERTIES OUTPUT_NAME "libargparse")

# set C++ standard to use
set_property(TARGET libargparse PROPERTY CXX_STANDARD 17)
set_property(TARGET libargparse PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET libargparse PROPERTY CXX_EXTENSIONS OFF)

# force linker language on the target
set_target_properties(libargparse PROPERTIES LINKER_LANGUAGE CXX)

# position independent code
set_property(TARGET libargparse PROPERTY POSITION_INDEPENDENT_CODE ON)

# include directories
target_include_directories(libargparse PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(libargparse_iface INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

# configure interface
target_link_libraries(libargparse_iface INTERFACE libargparse)

# add source files to target for compilation
set(LIBARGPARSE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/argparse")
target_sources(libargparse PRIVATE
    # public sources
    "${LIBARGPARSE_ROOT}/argparse.cpp"
    "${LIBARGPARSE_ROOT}/argparse.hpp"
    "${LIBARGPARSE_ROOT}/argument.hpp"

    # private sources
    "${LIBARGPARSE_ROOT}/internal/parser.cpp"
    "${LIBARGPARSE_ROOT}/internal/parser.hpp"
    "${LIBARGPARSE_ROOT}/internal/printer.cpp"
    "${LIBARGPARSE_ROOT}/internal/printer.hpp"
    "${LIBARGPARSE_ROOT}/internal/wcwidth.cpp"
    "${LIBARGPARSE_ROOT}/internal/wcwidth.hpp"
)

# disable runtime type information, not needed for this library
target_compile_options(libargparse PRIVATE -fno-rtti)

# make implicit function declarations an error
target_compile_options(libargparse PRIVATE -Werror=implicit-function-declaration)
