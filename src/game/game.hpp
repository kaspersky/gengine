#include <game.h>

namespace game {

template <typename IGame>
IBot<IGame>::IBot(const IGame *game)
{
    if (game != nullptr)
        this->game = new IGame(*game);
}

template <typename IGame>
IBot<IGame>::~IBot()
{
    delete game;
}

template <typename IGame>
void
IBot<IGame>::SendMove(const IMove &move)
{
    game->ApplyMove(move);
}

}
