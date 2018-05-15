#pragma once

#include <game.h>
#include <mcts.h>

namespace generic_bots {

class FixedMctsBot: public game::IBot
{
    long long num_iterations;

public:
    FixedMctsBot(const game::IGame *game, long long num_iterations);

    game::IMove MakeMove();

    game::IBot *Clone() const;
};

struct FixedMctsWithCachingBot: public game::IBot
{
    long long num_iterations;
    MCTSNode *root;

    FixedMctsWithCachingBot(const game::IGame *game, long long num_iterations, const MCTSNode *mcts_node);

    void AdvanceRoot(const game::IMove &move);

public:
    FixedMctsWithCachingBot(const game::IGame *game, long long num_iterations);

    game::IMove MakeMove();

    void SendMove(const game::IMove &move);

    game::IBot *Clone() const;
};

}
