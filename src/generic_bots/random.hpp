#include <generic_bots.h>

namespace generic_bots {

template <typename IGame>
RandomBot<IGame>::RandomBot(const IGame *game): game::IBot<IGame>(game)
{
}

template <typename IGame>
game::IBot<IGame> *
RandomBot<IGame>::Clone() const
{
    return new RandomBot(this->game);
}

template <typename IGame>
game::IMove
RandomBot<IGame>::MakeMove()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    auto moves = this->game->GetPossibleMoves();
    auto move = moves[std::uniform_int_distribution<>(0, moves.size() - 1)(gen)];
    this->game->ApplyMove(move);
    return move;
}

}