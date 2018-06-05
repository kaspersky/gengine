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

struct MCTSNode
{
    double value;
    long long total;
    int move_count;
    std::unordered_map<game::IMove, MCTSNode *> children;

    MCTSNode(int move_count): value(0.0), total(0), move_count(move_count)
    {
    }

    MCTSNode(const MCTSNode &other): value(other.value), total(other.total)
    {
        for (auto it : other.children)
            children.emplace(it.first, new MCTSNode(*it.second));
    }

    ~MCTSNode()
    {
        for (auto it : children)
            delete it.second;
    }
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
CountUnique(const MCTSNode &root, const IGame &game);

#include "mcts.hpp"
#include "mcts_parallel.hpp"
#include "mcts_cache.hpp"