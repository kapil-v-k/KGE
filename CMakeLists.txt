cmake_minimum_required(VERSION 3.10)
project(rooster CXX C)

#Source Files
set(SOURCE_FILES 
    src/main.cpp
    src/glad.c
    src/Application.cpp
    src/UiObject.cpp
    src/Primitive.cpp
    src/Shader.cpp
    src/Font.cpp
    src/Texture.cpp
    src/Clock.cpp
    src/opengl/HardwareScissorLock.cpp
    src/Viewport.cpp
    src/RenderComponent.cpp
    src/ClipContainerComponent.cpp
    src/TextComponent.cpp
    )
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

    # Include glad
add_library(glad STATIC "src/glad.c")
target_include_directories(glad PUBLIC "include")


# --- GLFW (Imported Shared Library) ---
add_library(glfw SHARED IMPORTED)
set_target_properties(glfw PROPERTIES
    IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/libglfw3dll.a"
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/src/glfw3.dll"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include/GLFW"
)

# 1. Define FreeType as an imported library
# Use STATIC if you have .lib/.a, or SHARED if you have a .dll
add_library(freetype STATIC IMPORTED)

# 2. Set the path to the library file
set_target_properties(freetype PROPERTIES
   # IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/libfreetype.a"
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/libfreetype.a"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include/freetype"
)



add_executable(${PROJECT_NAME} ${SOURCE_FILES})

set_target_properties(rooster PROPERTIES
    # For Visual Studio & VS Code CMake Tools
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    # For general debuggers (CMake 3.25+)
    DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(rooster PRIVATE glfw glad freetype)

target_include_directories(rooster PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include # Adds the 'include' subdirectory
    ${CMAKE_CURRENT_SOURCE_DIR}/include/freetype/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/glad
    ${CMAKE_CURRENT_SOURCE_DIR}/include/GLFW
    ${CMAKE_CURRENT_SOURCE_DIR}/include/KHR
)

# Automatically copy the assets directory into the build output folder on every build
add_custom_command(TARGET rooster POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/src/assets"
    "${CMAKE_CURRENT_BINARY_DIR}/assets"
    COMMENT "Deploying UI shader asset assets straight to build execution directory..."
)
