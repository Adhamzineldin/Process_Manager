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


# Install man pages
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/man/process_manager_gui.1
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/man/man1)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/man/process_manager_cli.1
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/man/man1)

# Add a custom target for man page testing
add_custom_target(man_gui
        COMMAND man -l ${CMAKE_CURRENT_SOURCE_DIR}/man/process_manager_gui.1
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Viewing GUI man page")

add_custom_target(man_cli
        COMMAND man -l ${CMAKE_CURRENT_SOURCE_DIR}/man/process_manager_cli.1
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Viewing CLI man page")