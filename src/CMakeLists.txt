include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/src/game")
include_directories("${PROJECT_SOURCE_DIR}/src/mcts")
include_directories("${PROJECT_SOURCE_DIR}/src/manager")

add_executable(main main.cpp uttt.cpp)
target_link_libraries(main mcts game manager)