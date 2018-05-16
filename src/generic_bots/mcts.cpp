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
    MCTSNode node(game);
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

FixedMcts01Bot::FixedMcts01Bot(const game::IGame *game, long long num_iterations): game::IBot(game), num_iterations(num_iterations)
{
}

game::IMove
FixedMcts01Bot::MakeMove()
{
    MCTSNode node(game);
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

    game->ApplyMove(move);

    return move;
}

game::IBot *
FixedMcts01Bot::Clone() const
{
    return new FixedMcts01Bot(game, num_iterations);
}

FixedMctsWithCachingBot::FixedMctsWithCachingBot(const game::IGame *game, long long num_iterations, const MCTSNode *mcts_node): game::IBot(game), num_iterations(num_iterations), root(nullptr)
{
    if (mcts_node != nullptr)
        root = new MCTSNode(*mcts_node);
}

FixedMctsWithCachingBot::FixedMctsWithCachingBot(const game::IGame *game, long long num_iterations): game::IBot(game), num_iterations(num_iterations)
{
    root = new MCTSNode(game);
}

void
FixedMctsWithCachingBot::AdvanceRoot(const game::IMove &move)
{
    MCTSNode *new_root = root->children[move];
    root->children[move] = nullptr;
    delete root;
    root = new_root;
    if (root == nullptr)
        root = new MCTSNode(game);
}

game::IMove
FixedMctsWithCachingBot::MakeMove()
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

    game->ApplyMove(move);

    AdvanceRoot(move);

    return move;
}

void
FixedMctsWithCachingBot::SendMove(const game::IMove &move)
{
    game->ApplyMove(move);
    AdvanceRoot(move);
}

game::IBot *
FixedMctsWithCachingBot::Clone() const
{
    return new FixedMctsWithCachingBot(game, num_iterations, root);
}

}
