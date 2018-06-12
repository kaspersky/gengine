#include <mcts.h>
#include "uttt.h"
#include "utttp.h"

namespace uttt {

UtttBot::UtttBot(long long mcts_iterations): game::IBot<IBoard>(nullptr), mcts_iterations(mcts_iterations)
{
}

UtttBot::UtttBot(const IBoard &board, long long mcts_iterations): game::IBot<IBoard>(nullptr), board(board), mcts_iterations(mcts_iterations)
{
}

UtttBot::UtttBot(const UtttBot &other): game::IBot<IBoard>(other), board(other.board), mcts_iterations(other.mcts_iterations)
{
}

game::IMove
UtttBot::MakeMove()
{
    auto results = MCTS<IBoard>(board, mcts_iterations);
    double max = -1.1;
    game::IMove move = -1;
    for (const auto &result : results)
    {
        auto v = result.value / result.total;
        if (v > max)
            max = v, move = result.move;
    }
    board.ApplyMove(move);
    return move;
}

void
UtttBot::SendMove(const game::IMove &move)
{
    board.ApplyMove(move);
}

game::IBot<IBoard> *
UtttBot::Clone() const
{
    return new UtttBot(*this);
}

}