# CMakeLists.txt

cmake_minimum_required(VERSION 3.0)
project(vjidi)

find_path(LUA_INCLUDE_DIR lua.h)

find_library(LIBLUA lua)

add_executable(vjidi vjidi.c)
target_include_directories(vjidi SYSTEM PUBLIC ${LUA_INCLUDE_DIR})
target_link_libraries(vjidi ${LIBLUA})
