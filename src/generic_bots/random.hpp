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
    return new RandomBot(game);
}

template <typename IGame>
game::IMove
RandomBot<IGame>::MakeMove()
{
    auto move = game->GetRandomMove();
    game->ApplyMove(move);
    return move;
}

}
