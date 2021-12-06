cmake_minimum_required(VERSION 3.20)
project(untitled5)

set(CMAKE_CXX_STANDARD 14)

add_executable(untitled5_server main.cpp ServerNetwork.cpp ServerNetwork.hpp ClientNetwork.cpp ClientNetwork.hpp Command.cpp Draw.cpp)
add_executable(untitled5_client1 main.cpp ServerNetwork.cpp ServerNetwork.hpp ClientNetwork.cpp ClientNetwork.hpp Command.cpp Command.hpp Draw.cpp Draw.hpp)
add_executable(untitled5_client2 main.cpp ServerNetwork.cpp ServerNetwork.hpp ClientNetwork.cpp ClientNetwork.hpp Command.cpp Draw.cpp)

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR C:/SFML/lib/cmake/SFML)
find_package(SFML COMPONENTS system window graphics audio network REQUIRED)

include_directories(c:/SFML/include)

target_link_libraries(untitled5_server sfml-system sfml-window sfml-graphics sfml-audio sfml-network)
target_link_libraries(untitled5_client1 sfml-system sfml-window sfml-graphics sfml-audio sfml-network)
target_link_libraries(untitled5_client2 sfml-system sfml-window sfml-graphics sfml-audio sfml-network)

