#pragma once

#include <game.h>

namespace generic_bots {

class FixedMctsBot: public game::IBot
{
    long long num_iterations;

public:
    FixedMctsBot(const game::IGame *game, long long num_iterations);

    game::IMove MakeMove();

    game::IBot *Clone() const;
};

}
