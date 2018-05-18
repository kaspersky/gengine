#include <generic_bots.h>

namespace generic_bots {

template <typename IGame>
RandomBot<IGame>::RandomBot(const IGame *game): game::IBot<IGame>(game)
{
}

template <typename IGame>
game::IBot<IGame> *
RandomBot<IGame>::Clone() const
{
    return new RandomBot(this->game);
}

template <typename IGame>
game::IMove
RandomBot<IGame>::MakeMove()
{
    auto move = this->game->GetRandomMove();
    this->game->ApplyMove(move);
    return move;
}

}
