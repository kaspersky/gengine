#include <algorithm>

#include "minimax.h"

template <typename IGame, typename Eval>
static double
ABeta_(const IGame &game, int depth, double alfa, double beta)
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
        auto r = -ABeta_<IGame, Eval>(new_game, depth - 1, -beta, -alfa);
        if (!result_set || r > result)
            result = r, result_set = true;
        alfa = std::max(alfa, r);
        if (alfa >= beta)
            break;
    }

    return result;
}

namespace minimax {

template <typename IGame, typename Eval>
std::vector<std::pair<game::IMove, double>>
ABeta<IGame, Eval>::operator()(const IGame &game, int depth) const
{
    if (depth <= 0)
        return {};

    auto moves = game.GetPossibleMoves();
    std::vector<std::future<std::pair<game::IMove, double>>> futures;

    for (auto move : moves)
        futures.emplace_back(std::async(std::launch::async, [game, depth, move] {
            IGame new_game(game);
            new_game.ApplyMove(move);
            return std::pair<game::IMove, double>{move, -ABeta_<IGame, Eval>(new_game, depth - 1, -std::numeric_limits<double>::max(), std::numeric_limits<double>::max())};
        }));

    std::vector<std::pair<game::IMove, double>> results;
    for (auto &f : futures)
    {
        auto r = f.get();
        results.emplace_back(r.first, r.second);
    }

    std::sort(std::begin(results), std::end(results), [](const std::pair<game::IMove, double> &e1, const std::pair<game::IMove, double> &e2) {
        return e1.second > e2.second;
    });
    return results;
}

}