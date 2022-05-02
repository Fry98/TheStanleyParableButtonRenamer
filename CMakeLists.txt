cmake_minimum_required(VERSION 3.5)
project(TSPBR LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${PROJECT_NAME} main.cpp app.rc)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "The Stanley Parable Ultra Deluxe")
target_link_options(${PROJECT_NAME} PUBLIC /SUBSYSTEM:windows /ENTRY:mainCRTStartup)

add_executable(${PROJECT_NAME}_UNINST uninstall.cpp)
set_target_properties(${PROJECT_NAME}_UNINST PROPERTIES OUTPUT_NAME "TSPBR_uninstall")
target_link_options(${PROJECT_NAME}_UNINST PUBLIC /SUBSYSTEM:windows /ENTRY:mainCRTStartup)
