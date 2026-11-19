cmake_minimum_required(VERSION 3.16)
project(GeneticAlgorithm VERSION 0.1.0 LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Use an out-of-source build: cmake -S . -B build")
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(GA_BUILD_CORE "Build the thesis algorithm and settings (requires Qt 5)" ON)
option(GA_USE_LIBRARY_STUBS "Use placeholders for the private libraries" ON)
option(GA_CHECK_QT_STUBS "Also check the Qt-dependent placeholders" OFF)
include(CTest)

if(GA_BUILD_CORE)
    include(cmake/ProjectDependencies.cmake)
    add_library(genetic_algorithm STATIC GA.cpp GA.hpp settings.cpp settings.hpp)
    add_library(GeneticAlgorithm::Core ALIAS genetic_algorithm)
    target_include_directories(genetic_algorithm PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
    target_link_libraries(genetic_algorithm PUBLIC GeneticAlgorithm::Dependencies)
    if(MSVC)
        target_compile_options(genetic_algorithm PRIVATE /W4 /utf-8)
    else()
        target_compile_options(genetic_algorithm PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()

# The PDF contains controller listings, but not their .ui/.qrc files or main.cpp.
# Keep the files visible in IDEs without pretending to build a GUI application.
add_custom_target(thesis_gui_sources SOURCES
    routeplaner.cpp routeplaner.hpp zones.cpp zones.hpp)

if(BUILD_TESTING)
    add_subdirectory(stubs)
endif()
