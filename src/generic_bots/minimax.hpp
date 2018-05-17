#include <game.h>

namespace generic_bots {

template <typename IGame, typename Eval>
class MinimaxBot: public game::IBot<IGame>
{
    int depth;

    std::pair<double, game::IMove> Minimax(const IGame *game, int depth) const
    {
        Eval evaluator;
        if (depth == 0)
            return {evaluator(game), -1};
        auto moves = game->GetPossibleMoves();
        if (moves.empty())
            return {evaluator(game), -1};
        std::pair<double, game::IMove> result = {0.0, -1};
        for (auto m : moves)
        {
            auto new_game = new IGame(*game);
            new_game->ApplyMove(m);
            auto r = Minimax(new_game, depth - 1);
            delete new_game;
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
        auto r = Minimax(this->game, depth);
        this->game->ApplyMove(r.second);
        return r.second;
    }
};

}