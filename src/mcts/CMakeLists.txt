include_directories("${PROJECT_SOURCE_DIR}/src/mcts")
include_directories("${PROJECT_SOURCE_DIR}/src/game")

add_library(mcts mcts.cpp mcts2.cpp)