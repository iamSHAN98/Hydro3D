cmake_minimum_required(VERSION 3.10)
project(TreeMaker VERSION 1.0 LANGUAGES CXX)

# Set compile options
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Cross-check compile options
message("Using C++ standard ${CMAKE_CXX_STANDARD}")
message("Build mode : ${CMAKE_BUILD_TYPE}")
message("Build options : ${CMAKE_CXX_FLAGS_RELEASE}")

# External packages

# ROOT : Analysis
find_package(ROOT CONFIG REQUIRED)

# Source
include_directories(include)
set(SOURCE src/TreeMaker.cc)

# Generate Library
add_library(TreeMaker SHARED ${SOURCE})
target_include_directories(TreeMaker PRIVATE ${ROOT_INCLUDE_DIRS})
target_link_libraries(TreeMaker PRIVATE ${ROOT_LIBRARIES})

# Generate dictionary
ROOT_GENERATE_DICTIONARY(TreeMakerDict include/Object.h MODULE TreeMaker LINKDEF include/LinkDef.h)

# Generate Executable(s)
add_executable(maketree main.cc)
target_include_directories(maketree PRIVATE ${ROOT_INCLUDE_DIRS})
target_link_libraries(maketree PRIVATE ${ROOT_LIBRARIES} TreeMaker)
