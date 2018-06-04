#pragma once

namespace minimax {

template <typename IGame>
struct Eval
{
    double operator()(const IGame &game) const;
};

template <typename IGame, typename Eval=Eval<IGame>>
std::pair<double, game::IMove>
Minimax(const IGame &game, int depth);

template <typename IGame, typename Eval=Eval<IGame>>
std::pair<double, game::IMove>
ABeta(const IGame &game, int depth, double alfa, double beta);

}

#include "eval.hpp"
#include "minimax.hpp"
#include "abeta.hpp"