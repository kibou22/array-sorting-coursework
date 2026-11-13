cmake_minimum_required(VERSION 3.16)

project(SortingApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(SortingApp
    main.cpp
    mainwindow.h
    mainwindow.cpp
    arraygenerator.h
    arraygenerator.cpp
    sortingalgorithms.h
    sortingalgorithms.cpp
)

target_link_libraries(SortingApp PRIVATE Qt6::Widgets)