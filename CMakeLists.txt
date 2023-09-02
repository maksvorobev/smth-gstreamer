cmake_minimum_required(VERSION 3.5)

project(untitled9 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig)
pkg_search_module(gstreamer REQUIRED IMPORTED_TARGET gstreamer-1.0>=1.4)

add_executable(untitled9 main.cpp)

include(GNUInstallDirs)
install(TARGETS untitled9
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

target_link_libraries(untitled9
    PkgConfig::gstreamer
)
