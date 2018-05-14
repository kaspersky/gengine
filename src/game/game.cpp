#include <random>

#include <game.h>

namespace game {

IGame::~IGame()
{
}

IMove
IGame::GetRandomMove() const
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    auto moves = GetPossibleMoves();
    return moves[std::uniform_int_distribution<>(0, moves.size() - 1)(gen)];
}

int
IGame::GetMoveCount() const
{
    return GetPossibleMoves().size();
}

IBot::IBot(): game(nullptr)
{
}

IBot::~IBot()
{
}

void
IBot::Init(const IGame *game)
{
    if (game != nullptr)
        this->game = game->Clone();
}

void
IBot::SendMove(const IMove &move)
{
    game->ApplyMove(move);
}

}