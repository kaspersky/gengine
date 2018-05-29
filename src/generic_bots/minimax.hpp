#include <game.h>

namespace generic_bots {

template <typename IGame, typename Eval=Eval<IGame>>
class MinimaxBot: public game::IBot<IGame>
{
    int depth;

    std::pair<double, game::IMove> Minimax(const IGame &game, int depth) const
    {
        Eval evaluator;

        if (depth == 0)
            return {evaluator(game), -1};

        auto status = game.GetStatus();
        if (status != game::Undecided)
        {
            if (status == game::Draw)
                return {0.0, -1};
            if (status == game.GetPlayerToMove())
                return {std::numeric_limits<double>::max(), -1};
            return {-std::numeric_limits<double>::max(), -1};
        }

        std::pair<double, game::IMove> result = {0.0, -1};
        auto moves = game.GetPossibleMoves();
        for (auto m : moves)
        {
            IGame new_game(game);
            new_game.ApplyMove(m);
            auto r = Minimax(new_game, depth - 1);
            if (result.second == -1 || -r.first > result.first)
                result = {-r.first, m};
        }

        return result;
    }

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
        auto r = Minimax(*this->game, depth);
        this->game->ApplyMove(r.second);
        return r.second;
    }
};

}