cmake_minimum_required(VERSION 2.8.11)

project("radegen")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})


# select the platform you want
#include ( "${PROJECT_SOURCE_DIR}/src/x11_main/buildsettings.cmake" )
#include ( "${PROJECT_SOURCE_DIR}/src/win32_main/buildsettings.cmake" )
include ( "${PROJECT_SOURCE_DIR}/src/glfw_main/buildsettings.cmake" )
add_definitions(${PLATFORM_DEFINES})


set(SRC_PATH "${PROJECT_SOURCE_DIR}/src" ${SRC_PATH})

#main src
file(GLOB TEMPLATE_SRC
        "${PLATFORM_SRC_DIR}/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/display_gl/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/ext/imgui/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/ext/imgui_addons/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/common/*.cpp"
        )

#inlcudes
include_directories(
        "${PROJECT_SOURCE_DIR}/src"
        "${PROJECT_SOURCE_DIR}/src/display_gl"
        "${PROJECT_SOURCE_DIR}/src/ext/imgui"
        "${PROJECT_SOURCE_DIR}/src/ext/imgui_addons"
        "${PROJECT_SOURCE_DIR}/src/ext"
        "${PROJECT_SOURCE_DIR}/src/common"
        "${PLATFORM_SRC_DIR}"
)

add_executable( ${CMAKE_PROJECT_NAME} ${PLATFORM_SRC} ${TEMPLATE_SRC})

target_link_libraries( ${CMAKE_PROJECT_NAME} ${PLATFORM_LINKS} )

if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC _DEBUG=1)
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC _DEBUG=0)
endif()


