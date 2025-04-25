cmake_minimum_required(VERSION 3.10)
project(process_manager C)

# Find GTK package
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

add_executable(procmanager-gui procmanager-gui.c)
target_link_libraries(procmanager-gui ${GTK3_LIBRARIES})