cmake_minimum_required(VERSION 2.8.12)
project(Dlib_tests)
set(cmake_cxx_flags "-std=c++14 -O3 -march=native")
include(/home/rohan/Downloads/Compressed/dlib-19.16/dlib/cmake)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include)

macro(add_code name)
   add_executable(${name} ${CMAKE_CURRENT_SOURCE_DIR}/src/${name}.cpp)
   target_link_libraries(${name} dlib::dlib )
   target_link_libraries(${name} ${OpenCV_LIBS} )
   target_link_libraries(${name} Xtst X11)
endmacro()

add_code(test1)
add_code(test2)
add_code(test3_Abhirup)
