cmake_minimum_required(VERSION 3.5 )
project(TestZoon)

set(CMAKE_CXX_STANDARD 14)

add_executable(TestZoon main.cpp serial_port.cpp  mesh.h circular_buffer.h)
add_executable(TestMesh mesh_test.cpp serial_port.cpp  mesh.h circular_buffer.h )

target_link_libraries(TestZoon   pthread )
target_link_libraries(TestMesh   pthread )