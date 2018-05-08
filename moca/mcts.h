#pragma once

#include <unordered_map>

#include <game.h>

namespace std
{
    template<>
    struct hash<const IGame *>
    {
        std::size_t operator()(const IGame *game) const
        {
            return game->Hash();
        }
    };

    template<>
    struct equal_to<const IGame *>
    {
        bool operator()(const IGame *game1, const IGame *game2) const
        {
            return game1->Equal(game2);
        }
    };
}

struct MCTSNode
{
    IGame *game;
    double value;
    int total;
    std::unordered_map<IMove, MCTSNode *> children;

    MCTSNode();

    ~MCTSNode();
};

void
MCTS(MCTSNode *root);

void
MCTS2(MCTSNode *root);
