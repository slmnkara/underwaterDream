cmake_minimum_required(VERSION 3.15)
project(UnderwaterDream)

# Raylib FetchContent (DO NOT CHANGE!)
include(FetchContent)
FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG        5.5
)
FetchContent_MakeAvailable(raylib)

# SOURCE CODES
set(SOURCES
    src/main.c
    src/definitions.c
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} raylib)

# Copy assets to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

if(APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit" "-framework Cocoa" "-framework OpenGL")
elseif(WIN32)
    target_link_libraries(${PROJECT_NAME} winmm gdi32 opengl32 shell32)
elseif(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} m pthread dl rt)
endif()

# OS Linker (DO NOT CHANGE!)
if(APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit" "-framework Cocoa" "-framework OpenGL")
elseif(NOT MSVC)
    target_link_libraries(${PROJECT_NAME} m pthread dl)
endif()