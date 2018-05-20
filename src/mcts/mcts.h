#pragma once

#include <unordered_map>

#include <game.h>

template <typename IGame>
struct MCTSNode
{
    IGame *game;
    double value;
    int total;
    std::unordered_map<game::IMove, MCTSNode *> children;

    MCTSNode(const IGame *game);

    MCTSNode(const MCTSNode &other);

    ~MCTSNode();
};

template <typename IGame>
void
MCTS(MCTSNode<IGame> *root);

template <typename IGame>
void
MCTS01(MCTSNode<IGame> *root);

template <typename IGame>
void
MCTS_parallel(MCTSNode<IGame> *root);

template <typename IGame>
void
MCTS_cache(MCTSNode<IGame> *root);

template <typename IGame>
long long
CountUnique(const MCTSNode<IGame> *root);

#include <mcts.hpp>
#include <mcts_parallel.hpp>
#include <mcts_cache.hpp>