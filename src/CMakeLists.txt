cmake_minimum_required(VERSION 3.10)


file(GLOB CURRENT_HEADERS  *.h *.hpp)
source_group("Include" FILES ${CURRENT_HEADERS})
#aux_source_directory(. SRC_LIST)
add_executable(templatelib hexmodu.cpp worker.cpp ${CURRENT_HEADERS})
add_executable(templatematch templatematch.cpp hexmodu.cpp ${CURRENT_HEADERS})
