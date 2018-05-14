#include <generic_bots.h>
#include <game.h>
#include <mcts.h>

namespace generic_bots {

FixedMctsBot::FixedMctsBot(const game::IGame *game, long long num_iterations): game::IBot(game), num_iterations(num_iterations)
{
}

game::IMove
FixedMctsBot::MakeMove()
{
    MCTSNode node;
    node.game = game->Clone();
    for (int i = 0; i < num_iterations; ++i)
        MCTS(&node);

    double max = -1.1;
    game::IMove move = -1;
    for (auto it : node.children)
    {
        auto v = it.second->value / it.second->total;
        if (v > max)
            max = v, move = it.first;
    }

    game->ApplyMove(move);

    return move;
}

game::IBot *
FixedMctsBot::Clone() const
{
    return new FixedMctsBot(game, num_iterations);
}

}
