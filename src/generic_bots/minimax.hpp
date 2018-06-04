#include <game.h>
#include <minimax.h>

namespace generic_bots {

template <typename IGame, typename Eval=minimax::Eval<IGame>>
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
        auto r = minimax::Minimax<IGame, Eval>(*this->game, depth);
        this->game->ApplyMove(r.second);
        return r.second;
    }
};

}