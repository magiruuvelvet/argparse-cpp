# unit tests
CreateTargetFromPath(argparse-tests "tests" EXECUTABLE "argparse-tests" C++ 20)

# unit testing framework
target_include_directories(argparse-tests SYSTEM PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/bandit")

target_compile_options(argparse-tests PRIVATE -fno-rtti)

target_link_libraries(argparse-tests PRIVATE
    libs::argparse
)
