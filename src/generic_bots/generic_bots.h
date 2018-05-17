#pragma once

#include <game.h>
#include <mcts.h>

namespace generic_bots {

template <typename IGame>
class FixedMctsBot: public game::IBot<IGame>
{
    long long num_iterations;

public:
    FixedMctsBot(const IGame *game, long long num_iterations);

    game::IBot<IGame> *Clone() const;

    game::IMove MakeMove();
};

template <typename IGame>
class FixedMcts01Bot: public game::IBot<IGame>
{
    long long num_iterations;

public:
    FixedMcts01Bot(const IGame *game, long long num_iterations);

    game::IBot<IGame> *Clone() const;

    game::IMove MakeMove();
};

template <typename IGame>
struct FixedMctsWithCachingBot: public game::IBot<IGame>
{
    long long num_iterations;
    MCTSNode<IGame> *root;

    void AdvanceRoot(const game::IMove &move);

public:
    FixedMctsWithCachingBot(const IGame *game, long long num_iterations);

    FixedMctsWithCachingBot(const IGame *game, long long num_iterations, const MCTSNode<IGame> *root);

    game::IBot<IGame> *Clone() const;

    game::IMove MakeMove();

    void SendMove(const game::IMove &move);
};

template <typename IGame>
class RandomBot: public game::IBot<IGame>
{
    IGame *game;
public:
    RandomBot(const IGame *game);

    game::IBot<IGame> *Clone() const;

    game::IMove MakeMove();
};

}

#include "mcts.hpp"
#include "random.hpp"
#include "minimax.hpp"