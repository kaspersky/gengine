#include "minimax.h"

template <typename IGame, typename Eval>
static std::pair<double, game::IMove>
Minimax_(const IGame &game, int depth)
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
        auto r = Minimax_<IGame, Eval>(new_game, depth - 1);
        if (result.second == -1 || -r.first > result.first)
            result = {-r.first, m};
    }

    return result;
}

namespace minimax {

template <typename IGame, typename Eval>
std::pair<double, game::IMove>
Minimax<IGame, Eval>::operator()(const IGame &game, int depth) const
{
    return Minimax_(game, depth);
}

}