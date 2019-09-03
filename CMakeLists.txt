# CMakeLists.txt

cmake_minimum_required(VERSION 3.0)
project(vjidi)

#####

find_path(LUA_INCLUDE_DIR lua.h)
find_library(LIBLUA lua53)

add_executable(vjidi vjidi.c)
target_include_directories(vjidi SYSTEM PUBLIC ${LUA_INCLUDE_DIR})
target_link_libraries(vjidi ${LIBLUA})

#####

add_executable(vjidiin vjidiin.c)

#####

add_library(luamidi SHARED luamidi.c)

find_library(LIBLUAJIT luajit-5.1)
target_include_directories(luamidi SYSTEM PUBLIC /usr/pkg/include) # XXX

target_link_libraries(luamidi ${LIBLUAJIT} m)
