cmake_minimum_required(VERSION "3.16")

# DepthAI Library
add_subdirectory(depthai-core EXCLUDE_FROM_ALL)

# Project declaration
project(oakd_opencv)
set(CMAKE_BUILD_TYPE Debug)

find_package(OpenCV)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

# Output dirs
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Target name
set(TARGET_NAME oakd_opencv)

# Add main target
add_executable(${TARGET_NAME}
    src/main.cpp
    src/util.cpp
)
target_link_libraries(${TARGET_NAME} PUBLIC
    ${OpenCV_LIBS}
    depthai-core
)