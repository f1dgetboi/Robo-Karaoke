cmake_minimum_required(VERSION 3.31.6)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Karaoke)
include_directories(
    ${PROJECT_SOURCE_DIR}/vendor/include
    ${PROJECT_SOURCE_DIR}/src/headers
    /opt/homebrew/include
)
# Link GLFW library
link_directories(
    ${PROJECT_SOURCE_DIR}/vendor/library
    /opt/homebrew/lib
)

set(SRC_FILES
    ${PROJECT_SOURCE_DIR}/src/main.cpp
    ${PROJECT_SOURCE_DIR}/src/vgkf.cpp
    ${PROJECT_SOURCE_DIR}/src/glad.c
    ${PROJECT_SOURCE_DIR}/src/EBO.cpp
    ${PROJECT_SOURCE_DIR}/src/VBO.cpp
    ${PROJECT_SOURCE_DIR}/src/VAO.cpp
    ${PROJECT_SOURCE_DIR}/src/stb.cpp
    ${PROJECT_SOURCE_DIR}/src/shaderClass.cpp
    ${PROJECT_SOURCE_DIR}/src/Texture.cpp
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_compile_definitions(${PROJECT_NAME} PRIVATE
    PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}"
)

# On macOS, you also need to link to Cocoa, OpenGL, etc.
if(APPLE)
    target_link_libraries(${PROJECT_NAME}
        glfw
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )

else()
    # Linux/Windows (modify as needed)
    target_link_libraries(${PROJECT_NAME} glfw GL)
endif()
