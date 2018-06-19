include_directories("${PROJECT_SOURCE_DIR}/src/uttt")
include_directories("${PROJECT_SOURCE_DIR}/src/game")
include_directories("${PROJECT_SOURCE_DIR}/src/mcts")

add_library(uttt board.cpp bot.cpp eval.cpp)
