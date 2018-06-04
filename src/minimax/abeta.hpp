#include "minimax.h"

template <typename IGame, typename Eval>
static std::pair<double, game::IMove>
ABeta_(const IGame &game, int depth, double alfa, double beta)
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
        auto r = ABeta_<IGame, Eval>(new_game, depth - 1, -beta, -alfa);
        if (result.second == -1 || -r.first > result.first)
            result = {-r.first, m};
        alfa = std::max(alfa, -r.first);
        if (alfa >= beta)
            break;
    }

    return result;
}

namespace minimax {

template <typename IGame, typename Eval>
std::pair<double, game::IMove>
ABeta<IGame, Eval>::operator()(const IGame &game, int depth) const
{
    return ABeta_<IGame, Eval>(game, depth, -std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
}

}