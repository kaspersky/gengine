#pragma once

namespace minimax {

template <typename IGame>
struct Eval;

template <typename IGame, typename Eval=Eval<IGame>>
struct Minimax
{
    std::pair<double, game::IMove> operator()(const IGame &game, int depth) const;
};

template <typename IGame, typename Eval=Eval<IGame>>
struct ABeta
{
    std::pair<double, game::IMove> operator()(const IGame &game, int depth) const;
};

template <typename IGame>
struct Eval
{
    double operator()(const IGame &game) const;
};

}

#include "eval.hpp"
#include "minimax.hpp"
#include "abeta.hpp"