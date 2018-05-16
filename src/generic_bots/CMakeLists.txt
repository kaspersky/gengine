include_directories("${PROJECT_SOURCE_DIR}/src/generic_bots")
include_directories("${PROJECT_SOURCE_DIR}/src/game")
include_directories("${PROJECT_SOURCE_DIR}/src/mcts")

add_library(generic_bots mcts.cpp random.cpp)
