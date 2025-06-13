cmake_minimum_required(VERSION 3.10)
project(OpenCL_SDL2)

set(CMAKE_CXX_STANDARD 17)

add_executable(SiRender
    src/SrdApp/main.cpp
    src/SrdApp/shader.cpp
)

target_include_directories(SiRender PRIVATE
    include
)

find_package(SDL2 REQUIRED)
target_include_directories(SiRender PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(SiRender PRIVATE ${SDL2_LIBRARIES})

find_package(OpenCL REQUIRED)
target_include_directories(SiRender PRIVATE ${OpenCL_INCLUDE_DIRS})
target_link_libraries(SiRender PRIVATE ${OpenCL_LIBRARIES})
