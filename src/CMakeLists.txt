include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/src/game")
include_directories("${PROJECT_SOURCE_DIR}/src/mcts")
include_directories("${PROJECT_SOURCE_DIR}/src/manager")
include_directories("${PROJECT_SOURCE_DIR}/src/uttt")

add_executable(main main.cpp)
target_link_libraries(main mcts game manager uttt)
