cmake_minimum_required(VERSION 3.15)
project(raylibtest C)

file(COPY assets DESTINATION ./)
set(CMAKE_C_STANDARD 99)

set(OPTIMIZATION_FLAGS -O0)

link_directories ("${PROJECT_SOURCE_DIR}/lib/static")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OPTIMIZATION_FLAGS} -lflecs_static -lraylib -framework OpenGL -framework GLUT -framework Cocoa -framework CoreVideo -framework IOKit -v")

FILE(GLOB sources src/*)
FILE(GLOB spine_sources lib/src/spine/*)
add_executable(raylibtest ${sources} ${spine_sources})
target_include_directories(raylibtest PRIVATE lib/include)