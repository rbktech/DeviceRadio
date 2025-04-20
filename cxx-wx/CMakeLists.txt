cmake_minimum_required(VERSION 3.10)

file(STRINGS "version" VERSION)

project(wxDeviceRadio VERSION ${VERSION})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(wxWidgets_LIB_DIR $ENV{WXWIN}/lib/gcc_libx64)

find_package(wxWidgets 3.2.1 REQUIRED propgrid xml core base)

#add_definitions(-DCOMMIT="${GIT_HASH}")
add_definitions(-DVERSION="${PROJECT_VERSION}")
add_definitions(-DHAVE_W32API_H)
add_definitions(-D__WXMSW__)
add_definitions(-D_UNICODE)

if(CMAKE_BUILD_TYPE MATCHES Debug)

elseif(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    add_definitions(-DNDEBUG)
else()
    add_definitions(-DNDEBUG)
endif()

include_directories("src")
include_directories("$ENV{RTLSDR}/include")
include_directories("$ENV{GPPanel}/include")
include_directories("${wxWidgets_INCLUDE_DIRS}")

add_compile_options(-funsigned-bitfields)
add_compile_options(-O0)
add_compile_options(-Wall)
add_compile_options(-mthreads)
add_compile_options(-Wno-ctor-dtor-privacy)
add_compile_options(-pipe)
add_compile_options(-fmessage-length=0)
add_compile_options(-mno-ms-bitfields)

add_link_options(-mthreads)
add_link_options(-static-libgcc)
add_link_options(-static-libstdc++)

if(CMAKE_BUILD_TYPE MATCHES Debug)

elseif(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    add_link_options(-s)
else()
    add_link_options(-mwindows)
    add_link_options(-s)
endif()

link_directories(${wxWidgets_LIB_DIR})
link_directories("$ENV{RTLSDR}/lib")
link_directories("$ENV{LIBUSB}/MinGW64/static")
link_directories("$ENV{GPPanel}/lib/gcc_libx64")

link_libraries(rtlsdr_static)
link_libraries(usb-1.0)

add_executable(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    OUTPUT_NAME DeviceRadio-${CMAKE_BUILD_TYPE}-${PROJECT_VERSION}
)

target_sources(${PROJECT_NAME} PRIVATE src/res/resource.rc)
target_sources(${PROJECT_NAME} PRIVATE src/main.cpp)
target_sources(${PROJECT_NAME} PRIVATE src/mainframe.cpp)
target_sources(${PROJECT_NAME} PRIVATE src/sdrblunt.cpp)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_link_libraries(${PROJECT_NAME} PRIVATE wxmsw32u_propgrid)
    target_link_libraries(${PROJECT_NAME} PRIVATE wxmsw32u_richtext)
    target_link_libraries(${PROJECT_NAME} PRIVATE wxmsw32u_xrc)
    target_link_libraries(${PROJECT_NAME} PRIVATE wxmsw32u_aui)
    target_link_libraries(${PROJECT_NAME} PRIVATE wxmsw32u_html)
    target_link_libraries(${PROJECT_NAME} PRIVATE wxmsw32u_adv)
    target_link_libraries(${PROJECT_NAME} PRIVATE wxmsw32u_core)
    target_link_libraries(${PROJECT_NAME} PRIVATE wxbase32u_xml)
    target_link_libraries(${PROJECT_NAME} PRIVATE wxbase32u_net)
    target_link_libraries(${PROJECT_NAME} PRIVATE wxbase32u)
    target_link_libraries(${PROJECT_NAME} PRIVATE wxscintilla)
    target_link_libraries(${PROJECT_NAME} PRIVATE wxtiff)
    target_link_libraries(${PROJECT_NAME} PRIVATE wxjpeg)
    target_link_libraries(${PROJECT_NAME} PRIVATE wxpng)
    target_link_libraries(${PROJECT_NAME} PRIVATE wxzlib)
    target_link_libraries(${PROJECT_NAME} PRIVATE wxregexu)
    target_link_libraries(${PROJECT_NAME} PRIVATE wxexpat)
    target_link_libraries(${PROJECT_NAME} PRIVATE kernel32)
    target_link_libraries(${PROJECT_NAME} PRIVATE user32)
    target_link_libraries(${PROJECT_NAME} PRIVATE gdi32)
    target_link_libraries(${PROJECT_NAME} PRIVATE comdlg32)
    target_link_libraries(${PROJECT_NAME} PRIVATE wxregexu)
    target_link_libraries(${PROJECT_NAME} PRIVATE winspool)
    target_link_libraries(${PROJECT_NAME} PRIVATE winmm)
    target_link_libraries(${PROJECT_NAME} PRIVATE shell32)
    target_link_libraries(${PROJECT_NAME} PRIVATE comctl32)
    target_link_libraries(${PROJECT_NAME} PRIVATE version)
    target_link_libraries(${PROJECT_NAME} PRIVATE shlwapi)
    target_link_libraries(${PROJECT_NAME} PRIVATE ole32)
    target_link_libraries(${PROJECT_NAME} PRIVATE oleaut32)
    target_link_libraries(${PROJECT_NAME} PRIVATE uuid)
    target_link_libraries(${PROJECT_NAME} PRIVATE rpcrt4)
    target_link_libraries(${PROJECT_NAME} PRIVATE advapi32)
    target_link_libraries(${PROJECT_NAME} PRIVATE wsock32)
    target_link_libraries(${PROJECT_NAME} PRIVATE uxtheme)
    target_link_libraries(${PROJECT_NAME} PRIVATE oleacc)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE ${wxWidgets_LIBRARIES})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE gpPanel)