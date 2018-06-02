#pragma once

#include <unordered_map>
#include <vector>

#include <game.h>

struct MCTSResults
{
    game::IMove move;
    double value;
    long long total;

    MCTSResults(game::IMove move, double value, long long total): move(move), value(value), total(total)
    {
    }

    MCTSResults(MCTSResults &&other): move(other.move), value(other.value), total(other.total)
    {
    }
};

template <typename IGame>
struct MCTSNode
{
    IGame game;
    double value;
    long long total;
    std::unordered_map<game::IMove, MCTSNode *> children;

    MCTSNode(const IGame &game);

    MCTSNode(const MCTSNode &other);

    ~MCTSNode();
};

template <typename IGame>
struct RandomPlayout
{
    int operator()(const IGame &game) const;
};

template <typename IGame, typename RandomPlayout=RandomPlayout<IGame>>
struct MCTS
{
    std::vector<MCTSResults> operator()(const IGame &game, long long iterations) const;
};

template <typename IGame, typename RandomPlayout=RandomPlayout<IGame>>
struct MCTS01
{
    std::vector<MCTSResults> operator()(const IGame &game, long long iterations) const;
};

template <typename IGame, typename RandomPlayout=RandomPlayout<IGame>>
struct MCTS_parallel
{
    std::vector<MCTSResults> operator()(const IGame &game, long long iterations) const;
};

template <typename IGame, typename RandomPlayout=RandomPlayout<IGame>>
struct MCTS_cache
{
    std::vector<MCTSResults> operator()(const IGame &game, long long iterations) const;
};

template <typename IGame>
long long
CountUnique(const MCTSNode<IGame> *root);

#include "mcts.hpp"
#include "mcts_parallel.hpp"
#include "mcts_cache.hpp"