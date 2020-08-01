project(ENVYTOOLS C)
cmake_minimum_required(VERSION 2.6)

# <3 libxml2
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-pointer-sign")

find_package(LibXml2 REQUIRED)

find_path(LIBICONV_INCLUDE_DIR iconv.h)
include_directories(${LIBXML2_INCLUDE_DIR} ${LIBICONV_INCLUDE_DIR})

configure_file(rnn_path.h.in rnn_path.h ESCAPE_QUOTES)
include_directories(${PROJECT_BINARY_DIR})

add_library(rnn rnn.c rnndec.c)

add_executable(headergen2 headergen2.c)

target_link_libraries(rnn ${LIBXML2_LIBRARIES} envyutil)
target_link_libraries(headergen2 rnn)

install(TARGETS headergen2 rnn
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib${LIB_SUFFIX}
	ARCHIVE DESTINATION lib${LIB_SUFFIX})
