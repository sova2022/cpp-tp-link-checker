# CMakeList.txt : CMake project for cpp-tp-link-checker, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.16)

project(tp_link_checker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network WebEngineWidgets)

qt_standard_project_setup()

# Add source to this project's executable.
add_executable (tp_link_checker 
	src/main.cpp
    src/parser_cli.h
    src/common.h
    src/url_handler.h
    src/url_handler.cpp  
    src/web_page.h
    src/logger.h
    src/urls_dispenser.h
    src/urls_dispenser.cpp
    src/web_page.cpp
)

# TODO: Add tests and install targets if needed.
target_link_libraries(tp_link_checker
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Network
        Qt6::WebEngineWidgets     
)
