include_directories("${PROJECT_SOURCE_DIR}/src/ttt")
include_directories("${PROJECT_SOURCE_DIR}/src/game")

add_library(ttt ttt.cpp)
target_link_libraries(ttt)