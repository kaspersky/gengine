#pragma once

#include <unordered_map>

#include <game.h>

namespace std
{
    template<>
    struct hash<const game::IGame *>
    {
        std::size_t operator()(const game::IGame *game) const
        {
            return game->Hash();
        }
    };

    template<>
    struct equal_to<const game::IGame *>
    {
        bool operator()(const game::IGame *game1, const game::IGame *game2) const
        {
            return game1->Equal(game2);
        }
    };
}

struct MCTSNode
{
    game::IGame *game;
    double value;
    int total;
    std::unordered_map<game::IMove, MCTSNode *> children;

    MCTSNode(const game::IGame *game);

    MCTSNode(const MCTSNode &other);

    ~MCTSNode();
};

void
MCTS(MCTSNode *root);

void
MCTS01(MCTSNode *root);

void
MCTS2(MCTSNode *root);

long long
CountUnique(const MCTSNode *root);