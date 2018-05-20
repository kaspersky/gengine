#pragma once

#include <unordered_map>
#include <vector>

#include <game.h>

template <typename IGame>
struct MCTSNode
{
    IGame *game;
    double value;
    long long total;
    std::unordered_map<game::IMove, MCTSNode *> children;

    MCTSNode(const IGame &game);

    MCTSNode(const MCTSNode &other);

    ~MCTSNode();
};

template <typename IGame>
std::vector<std::pair<game::IMove, std::pair<double, long long>>>
MCTS(const IGame &game, long long iterations);

template <typename IGame>
std::vector<std::pair<game::IMove, std::pair<double, long long>>>
MCTS01(const IGame &game, long long iterations);

template <typename IGame>
std::vector<std::pair<game::IMove, std::pair<double, long long>>>
MCTS_parallel(const IGame &game, long long iterations);

template <typename IGame>
std::vector<std::pair<game::IMove, std::pair<double, long long>>>
MCTS_cache(const IGame &game, long long iterations);

template <typename IGame>
long long
CountUnique(const MCTSNode<IGame> *root);

#include <mcts.hpp>
#include <mcts_parallel.hpp>
#include <mcts_cache.hpp>