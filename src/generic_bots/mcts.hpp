#include <game.h>
#include <mcts.h>

#include "generic_bots.h"

namespace generic_bots {

template <typename IGame, typename MCTS>
FixedMctsBot<IGame, MCTS>::FixedMctsBot(const IGame *game, const MCTS &mcts, long long num_iterations): game::IBot<IGame>(game), mcts(mcts), num_iterations(num_iterations)
{
}

template <typename IGame, typename MCTS>
game::IBot<IGame> *
FixedMctsBot<IGame, MCTS>::Clone() const
{
    return new FixedMctsBot(this->game, mcts, num_iterations);
}

template <typename IGame, typename MCTS>
game::IMove
FixedMctsBot<IGame, MCTS>::MakeMove()
{
    auto results = mcts(*this->game, num_iterations);

    double max = -1.1;
    game::IMove move = -1;
    for (const auto &result : results)
    {
        auto v = result.value / result.total;
        if (v > max)
            max = v, move = result.move;
    }

    this->game->ApplyMove(move);

    return move;
}

}