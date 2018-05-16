#include <generic_bots.h>

namespace generic_bots {

RandomBot::RandomBot(const game::IGame *game): game::IBot(game)
{
}

game::IMove
RandomBot::MakeMove()
{
    auto move = game->GetRandomMove();
    game->ApplyMove(move);
    return move;
}

game::IBot *
RandomBot::Clone() const
{
    return new RandomBot(game);
}

}
