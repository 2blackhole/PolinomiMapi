# qt_gui/CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(QT_SOURCES
        main.cpp
        MainWindow.cpp
        PolynomialWidget.cpp
        TableWidget.cpp
        ExpressionWidget.cpp
)

set(QT_HEADERS
        MainWindow.hpp
        PolynomialWidget.hpp
        TableWidget.hpp
        ExpressionWidget.hpp
)

add_executable(polynomial_qt_gui
        ${QT_SOURCES}
        ${QT_HEADERS}
)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

target_link_libraries(polynomial_qt_gui
        Qt6::Core
        Qt6::Widgets
        polinoms_lib
)

target_include_directories(polynomial_qt_gui PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
)

set_target_properties(polynomial_qt_gui PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)