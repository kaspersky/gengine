#include <game.h>
#include <mcts.h>

#include "generic_bots.h"

namespace generic_bots {

template <typename IGame, typename MCTS>
FixedMctsBot<IGame, MCTS>::FixedMctsBot(const IGame *game, long long num_iterations): game::IBot<IGame>(game), num_iterations(num_iterations)
{
}

template <typename IGame, typename MCTS>
game::IBot<IGame> *
FixedMctsBot<IGame, MCTS>::Clone() const
{
    return new FixedMctsBot(this->game, num_iterations);
}

template <typename IGame, typename MCTS>
game::IMove
FixedMctsBot<IGame, MCTS>::MakeMove()
{
    auto results = MCTS()(*this->game, num_iterations);

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

template <typename IGame>
FixedMctsWithCachingBot<IGame>::FixedMctsWithCachingBot(const IGame *game, long long num_iterations): game::IBot<IGame>(game), num_iterations(num_iterations)
{
    root = new MCTSNode(-1);
}

template <typename IGame>
FixedMctsWithCachingBot<IGame>::FixedMctsWithCachingBot(const IGame *game, long long num_iterations, const MCTSNode *root): game::IBot<IGame>(game), num_iterations(num_iterations)
{
    this->root = nullptr;
    if (root != nullptr)
        this->root = new MCTSNode(*root);
}

template <typename IGame>
game::IBot<IGame> *
FixedMctsWithCachingBot<IGame>::Clone() const
{
    return new FixedMctsWithCachingBot(this->game, num_iterations, root);
}

template <typename IGame>
void
FixedMctsWithCachingBot<IGame>::AdvanceRoot(const game::IMove &move)
{
    MCTSNode *new_root = root->children[move];
    root->children[move] = nullptr;
    delete root;
    root = new_root;
    if (root == nullptr)
        root = new MCTSNode(-1);
}

template <typename IGame>
game::IMove
FixedMctsWithCachingBot<IGame>::MakeMove()
{
    for (int i = 0; i < num_iterations; ++i)
        MCTS<IGame>()(root);

    double max = -1.1;
    game::IMove move = -1;
    for (const auto &it : root->children)
    {
        auto v = it.second->value / it.second->total;
        if (v > max)
            max = v, move = it.first;
    }

    this->game->ApplyMove(move);

    AdvanceRoot(move);

    return move;
}

template <typename IGame>
void
FixedMctsWithCachingBot<IGame>::SendMove(const game::IMove &move)
{
    this->game->ApplyMove(move);
    AdvanceRoot(move);
}

}