cmake_minimum_required(VERSION 3.10)
project(SimpleTextEditor)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find FLTK package
find_package(FLTK REQUIRED)

# Include FLTK directories
include_directories(${FLTK_INCLUDE_DIR})

# Add executable
add_executable(editor editor.cpp)

# Link FLTK libraries
target_link_libraries(editor ${FLTK_LIBRARIES})

# For Windows, set subsystem to windows to avoid console window
if(WIN32)
    set_target_properties(editor PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
