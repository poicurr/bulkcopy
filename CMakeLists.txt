set(CMAKE_CXX_STANDARD 17)

include_directories(
  PUBLIC ${PROJECT_SOURCE_DIR}/include/httpserver/
)

add_executable(main.bin main.cpp)


