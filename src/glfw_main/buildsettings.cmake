if(UNIX) 
    set( PLATFORM_LINKS GL X11 pthread dl glfw)
    set( PLATFORM_DEFINES "-DIMGUI_IMPL_OPENGL_LOADER_GLAD" )
else()
    include_directories("C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Tools\\MSVC\\14.28.29333\\include")
    link_directories("C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Tools\\MSVC\\14.28.29333\\lib\\x64")
    set( PLATFORM_LINKS winmm openGL32 glfw3)
    set( PLATFORM_DEFINES "-DIMGUI_IMPL_OPENGL_LOADER_GLAD -D_CRT_SECURE_NO_WARNINGS" )
endif()

set( PLATFORM_SRC_DIR "${PROJECT_SOURCE_DIR}/src/glfw_main" )

