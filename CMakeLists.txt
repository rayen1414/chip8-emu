cmake_minimum_required(VERSION 3.16)
project(Chip8Emulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "C:/Qt/6.11.0/mingw_64")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# ADD chip8.cpp HERE!
add_executable(Chip8Emulator
    main.cpp
    interface.hpp
    interface.cpp
    chip8.hpp
    chip8.cpp
)

target_link_libraries(Chip8Emulator PRIVATE Qt6::Widgets)