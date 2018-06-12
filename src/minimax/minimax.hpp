#include <algorithm>

#include "minimax.h"

namespace minimax {

template <typename IGame, typename Eval>
static double
Minimax_(const IGame &game, int depth)
{
    Eval evaluator;

    if (depth == 0)
        return evaluator(game);

    auto status = game.GetStatus();
    if (status != game::Undecided)
    {
        if (status == game::Draw)
            return 0.0;
        if (status == game.GetPlayerToMove())
            return std::numeric_limits<double>::max();
        return -std::numeric_limits<double>::max();
    }

    bool result_set = false;
    double result = 0.0;
    auto moves = game.GetPossibleMoves();
    for (auto m : moves)
    {
        IGame new_game(game);
        new_game.ApplyMove(m);
        auto r = -Minimax_<IGame, Eval>(new_game, depth - 1);
        if (!result_set || r > result)
            result = r, result_set = true;
    }

    return result;
}

template <typename IGame, typename Eval>
std::vector<std::pair<game::IMove, double>>
Minimax<IGame, Eval>::operator()(const IGame &game, int depth) const
{
    if (depth <= 0)
        return {};
    std::vector<std::pair<game::IMove, double>> results;
    auto moves = game.GetPossibleMoves();
    for (auto m : moves)
    {
        IGame new_game(game);
        new_game.ApplyMove(m);
        results.emplace_back(m, -Minimax_<IGame, Eval>(new_game, depth - 1));
    }
    std::sort(std::begin(results), std::end(results), [](const std::pair<game::IMove, double> &e1, const std::pair<game::IMove, double> &e2) {
        return e1.second > e2.second;
    });
    return results;
}

}