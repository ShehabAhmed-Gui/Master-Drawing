cmake_minimum_required(VERSION 3.16)
project(MasterDrawing VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick QuickControls2)
qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(MasterDrawing src/main.cpp)

qt_add_qml_module(MasterDrawing
    URI Draw
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/ToolBtn.qml
        qml/Swatches.qml
    SOURCES
        src/drawingcanvas.h
        src/drawingcanvas.cpp
)

target_link_libraries(MasterDrawing PRIVATE Qt6::Quick Qt6::QuickControls2)

target_include_directories(MasterDrawing PRIVATE src)
