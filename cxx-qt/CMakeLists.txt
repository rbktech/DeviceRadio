cmake_minimum_required(VERSION 3.10)

file(STRINGS "version" VERSION)

project(qtDeviceRadio VERSION ${VERSION})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_PREFIX_PATH "$ENV{LIBRARY}/devEngine")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(devEngine 0.2.0 REQUIRED)

find_package(Qt5 5.15.2 REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        Charts
)

include_directories("src")
include_directories("$ENV{GLM}/include")
include_directories("$ENV{GLEW}/include")
include_directories("$ENV{RTLSDR}/include")

add_definitions(-DVERSION="${PROJECT_VERSION}")
add_definitions(-DRESOURCES="${CMAKE_CURRENT_SOURCE_DIR}/res/")
add_definitions(-DGLEW_STATIC)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)
add_definitions(-DWIN32)
add_definitions(-DMINGW_HAS_SECURE_API=1)
add_definitions(-DQT_DEPRECATED_WARNINGS)
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_CORE_LIB)
add_definitions(-DQT_GUI_LIB)
add_definitions(-DQT_WIDGETS_LIB)
add_definitions(-DQT_WIDGETS_LIB)

#add_compile_options(-Wno-unknown-pragmas)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_compile_options(-fexceptions)
    add_compile_options(-Wextra)
    add_compile_options(-Wall)
else()
    add_compile_options(-O2)
endif()

if(NOT CMAKE_BUILD_TYPE MATCHES "Debug")
    add_link_options(-s)
    add_link_options(-mwindows)
endif()

#add_link_options(-static)

link_directories("$ENV{RTLSDR}/lib")
link_directories("$ENV{LIBUSB}/MinGW64/static")
link_directories("$ENV{GLEW}/lib")

add_executable(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME}-${CMAKE_BUILD_TYPE}-${PROJECT_VERSION})

target_sources(${PROJECT_NAME} PRIVATE src/main.cpp)
target_sources(${PROJECT_NAME} PRIVATE src/mainwindow.cpp)
target_sources(${PROJECT_NAME} PRIVATE src/glwidget.cpp)
target_sources(${PROJECT_NAME} PRIVATE src/rtlsdr.cpp)
target_sources(${PROJECT_NAME} PRIVATE src/linechart.cpp)
target_sources(${PROJECT_NAME} PRIVATE src/math/math_frequency.cpp)

target_link_libraries(${PROJECT_NAME}
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Charts
        -Wl,--whole-archive devGraphd -Wl,--no-whole-archive
        devCored
        glew32.a
        glu32
        opengl32
        rtlsdr_static
        usb-1.0
)