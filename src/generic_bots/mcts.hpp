#include <game.h>
#include <mcts.h>

#include "generic_bots.h"

namespace generic_bots {

template <typename IGame>
FixedMctsBot<IGame>::FixedMctsBot(const IGame *game, long long num_iterations): game::IBot<IGame>(game), num_iterations(num_iterations)
{
}

template <typename IGame>
game::IBot<IGame> *
FixedMctsBot<IGame>::Clone() const
{
    return new FixedMctsBot(this->game, num_iterations);
}

template <typename IGame>
game::IMove
FixedMctsBot<IGame>::MakeMove()
{
    MCTSNode<IGame> node(this->game);
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

    this->game->ApplyMove(move);

    return move;
}

template <typename IGame>
FixedMcts01Bot<IGame>::FixedMcts01Bot(const IGame *game, long long num_iterations): game::IBot<IGame>(game), num_iterations(num_iterations)
{
}

template <typename IGame>
game::IBot<IGame> *
FixedMcts01Bot<IGame>::Clone() const
{
    return new FixedMcts01Bot(this->game, num_iterations);
}

template <typename IGame>
game::IMove
FixedMcts01Bot<IGame>::MakeMove()
{
    MCTSNode<IGame> node(this->game);
    for (int i = 0; i < num_iterations; ++i)
        MCTS01(&node);

    double max = -1.1;
    game::IMove move = -1;
    for (auto it : node.children)
    {
        auto v = it.second->value / it.second->total;
        if (v > max)
            max = v, move = it.first;
    }

    this->game->ApplyMove(move);

    return move;
}

template <typename IGame>
FixedMctsWithCachingBot<IGame>::FixedMctsWithCachingBot(const IGame *game, long long num_iterations): game::IBot<IGame>(game), num_iterations(num_iterations)
{
    root = new MCTSNode<IGame>(game);
}

template <typename IGame>
FixedMctsWithCachingBot<IGame>::FixedMctsWithCachingBot(const IGame *game, long long num_iterations, const MCTSNode<IGame> *root): game::IBot<IGame>(game), num_iterations(num_iterations)
{
    this->root = nullptr;
    if (root != nullptr)
        this->root = new MCTSNode<IGame>(*root);
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
    MCTSNode<IGame> *new_root = root->children[move];
    root->children[move] = nullptr;
    delete root;
    root = new_root;
    if (root == nullptr)
        root = new MCTSNode<IGame>(this->game);
}

template <typename IGame>
game::IMove
FixedMctsWithCachingBot<IGame>::MakeMove()
{
    for (int i = 0; i < num_iterations; ++i)
        MCTS(root);

    double max = -1.1;
    game::IMove move = -1;
    for (auto it : root->children)
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
