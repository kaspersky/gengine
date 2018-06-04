#include <game.h>
#include <minimax.h>

namespace generic_bots {

template <typename IGame, typename Minimax>
class MinimaxBot: public game::IBot<IGame>
{
    int depth;

public:
    MinimaxBot(const IGame *game, int depth): game::IBot<IGame>(game), depth(depth)
    {
    }

    game::IBot<IGame> *Clone() const
    {
        return new MinimaxBot(this->game, depth);
    }

    game::IMove MakeMove()
    {
        auto move = Minimax()(*this->game, depth)[0].first;
        this->game->ApplyMove(move);
        return move;
    }
};

}