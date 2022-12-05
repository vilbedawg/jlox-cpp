cmake_minimum_required(VERSION 3.16)
project(BIS 
    LANGUAGES CXX
    VERSION 0.1.0
)

# C++20 is required
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)

if(CMAKE_PROJECT_NAME STREQUAL BIS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(tests)
endif()





