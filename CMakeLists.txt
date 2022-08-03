cmake_minimum_required(VERSION 3.0)
project(delay_message C)
set(VERSION 1.0.0)

set(CMAKE_C_STANDARD 11)

set(CMAKE_CXX_FLAGS)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

set(CMAKE_INSTALL_LIBDIR /usr/local/lib)

if(APPLE)
    set(CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS} -undefined dynamic_lookup")
endif(APPLE)

add_library(delay_message SHARED handler.c handler.h uthash.h plugin.c hash.c hash.h)

#target_link_libraries(gdb pq cjson)

#set_target_properties(gdb PROPERTIES VERSION 1.0.0 SOVERSION 1)

install(TARGETS delay_message LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})