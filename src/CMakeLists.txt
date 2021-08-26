cmake_minimum_required(VERSION 3.10)


file(GLOB CURRENT_HEADERS  *.h *.hpp)
source_group("Include" FILES ${CURRENT_HEADERS})
aux_source_directory(. SRC_LIST)
add_executable(templatelib ${SRC_LIST} ${CURRENT_HEADERS})
