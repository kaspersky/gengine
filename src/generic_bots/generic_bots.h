#pragma once

#include <game.h>
#include <mcts.h>

namespace generic_bots {

template <typename IGame, typename MCTS>
class FixedMctsBot: public game::IBot<IGame>
{
    MCTS mcts;
    long long num_iterations;

public:
    FixedMctsBot(const IGame *game, const MCTS &mcts, long long num_iterations);

    game::IBot<IGame> *Clone() const;

    game::IMove MakeMove();
};

template <typename IGame>
class RandomBot: public game::IBot<IGame>
{
public:
    RandomBot(const IGame *game);

    game::IBot<IGame> *Clone() const;

    game::IMove MakeMove();
};

}

#include "mcts.hpp"
#include "random.hpp"
#include "minimax.hpp"