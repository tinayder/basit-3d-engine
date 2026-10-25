cmake_minimum_required(VERSION 4.0)
project(basit)

set(CMAKE_CXX_STANDARD 20)

add_executable(basit basit.hpp
        main.cpp
)
include_directories(/usr/local/SFML-3.0.2/include)

find_package(SFML 3.0.2 COMPONENTS System Window Graphics Network Audio REQUIRED)
include_directories(${SFML_INCLUDE_DIRS})
target_link_libraries(basit SFML::System SFML::Window SFML::Graphics SFML::Audio SFML::Network)