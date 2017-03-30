# CMakeLists.txt

cmake_minimum_required(VERSION 3.0)
project(vjidi)

find_path(LUA_INCLUDE_DIR lua.h)
find_path(SDL2_INCLUDE_DIR SDL2/SDL.h)

find_library(LIBLUA lua)
find_library(LIBSDL2 SDL2)

add_executable(vjidi vjidi.c)
target_include_directories(vjidi SYSTEM PUBLIC ${LUA_INCLUDE_DIR} ${SDL2_INCLUDE_DIR})
target_link_libraries(vjidi ${LIBLUA} ${LIBSDL2})
