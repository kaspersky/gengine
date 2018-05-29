#include "generic_bots.h"

namespace generic_bots {

template <typename IGame>
double
Eval<IGame>::operator()(const IGame &game) const
{
    int status = game.GetStatus();
    if (status == game::Draw || status == game::Undecided)
        return 0.0;
    if (game.GetPlayerToMove() == status)
        return std::numeric_limits<double>::max();
    return -std::numeric_limits<double>::max();
}

}