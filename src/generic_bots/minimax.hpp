#include <game.h>

namespace generic_bots {

template <typename Eval>
class MinimaxBot: public game::IBot
{
    int depth;

    std::pair<double, game::IMove> Minimax(const game::IGame *game, int depth) const
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
            game::IGame *new_game = game->Clone();
            new_game->ApplyMove(m);
            auto r = Minimax(new_game, depth - 1);
            delete new_game;
            if (result.second == -1 || -r.first > result.first)
                result = {-r.first, m};
        }
        return result;
    }

public:
    MinimaxBot(const game::IGame *game, int depth): game::IBot(game), depth(depth)
    {
    }

    game::IMove MakeMove()
    {
        auto r = Minimax(game, depth);
        game->ApplyMove(r.second);
        return r.second;
    }

    game::IBot *Clone() const
    {
        return new MinimaxBot(game, depth);
    }
};

}