#pragma once

#include <vector>
#include <array>

#include <game.h>

namespace uttt {

class IBoard: public game::IGame
{
    int macro;
    std::array<short, 9> micro;
    char next;
    game::IPlayer player;

    void computeMacro();
    void updateMicro(const std::array<char, 81> &other);

public:
    IBoard(int macro, const std::array<short, 9> &micro, char next, char player);

    IBoard();

    bool operator==(const IBoard &other) const;

    std::vector<game::IMove> GetPossibleMoves() const;

    void ApplyMove(const game::IMove &move);

    int GetStatus() const;

    game::IPlayer GetPlayerToMove() const;

    IGame *Clone() const;

    std::size_t Hash() const;

    bool Equal(const game::IGame *game) const;

    void Print() const;
};

class UtttBot: public game::IBot
{
    IBoard board;
    long long mcts_iterations;

    UtttBot(const IBoard &board, long long mcts_iterations);

public:
    UtttBot(long long mcts_iterations);

    game::IMove MakeMove();

    void SendMove(const game::IMove &move);

    game::IBot *Clone() const;
};

}