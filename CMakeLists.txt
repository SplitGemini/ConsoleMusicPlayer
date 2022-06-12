# 
#  Created on Fri Jun 10 2022
# 
#  Copyright (C) SplitGemini
#  Author: SplitGemini
# 
cmake_minimum_required(VERSION 3.10)

project(Console-Music-Player VERSION 1.38.0 LANGUAGES CXX C)

find_package(libbass REQUIRED PATHS ${CMAKE_SOURCE_DIR})

set(CURSES_NEED_WIDE TRUE)
find_package(Curses REQUIRED)

# manually set source files
set(sources 
    MusicPlayer/Common.h
    MusicPlayer/Console.h
    MusicPlayer/Lyrics.h
    MusicPlayer/Main.cpp
    MusicPlayer/Player.h
    MusicPlayer/ini.h
)

add_executable(Console-Music-Player ${sources})
set_target_properties(Console-Music-Player
    PROPERTIES
        INSTALL_RPATH "\$ORIGIN"
        OUTPUT_NAME Console-Music-Player
)

# asio and websocketpp header
target_include_directories(Console-Music-Player
    PRIVATE
        ./bass
)

target_link_libraries(Console-Music-Player
    PRIVATE
        libbass
        ${CURSES_LIBRARIES}
)

install(TARGETS Console-Music-Player
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/install)

install(FILES bass/libbass.so
        DESTINATION ${CMAKE_SOURCE_DIR}/install)
        