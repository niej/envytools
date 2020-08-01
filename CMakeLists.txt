project(ENVYTOOLS C)
cmake_minimum_required(VERSION 2.6)
enable_testing()

SET(CMAKE_C_FLAGS "-Wall")

include_directories(rnn)

add_subdirectory(rnn)
add_subdirectory(afuc)
add_subdirectory(decode)

install(DIRECTORY include/ DESTINATION include/envytools)
install(DIRECTORY registers/ DESTINATION share)
install(DIRECTORY hwdocs DESTINATION share/doc/envytools)
install(FILES README DESTINATION share/doc/envytools)
