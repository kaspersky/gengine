#pragma once

#include <vector>
#include <array>

#include <game.h>

namespace uttt {

class IBoard
{
    int macro;
    std::array<short, 9> micro;
    char next;
    game::IPlayer player;

    void computeMacro();
    void updateMicro(const std::array<char, 81> &other);

public:
    IBoard();

    IBoard(int macro, const std::array<short, 9> &micro, char next, char player);

    IBoard(const IBoard &other);

    bool operator==(const IBoard &other) const;

    std::vector<game::IMove> GetPossibleMoves() const;

    int GetMoveCount() const;

    game::IMove GetRandomMove() const;

    void ApplyMove(const game::IMove &move);

    int GetStatus() const;

    game::IPlayer GetPlayerToMove() const;

    std::size_t Hash() const;

    bool Equal(const IBoard *game) const;

    void Print() const;
};

class UtttBot: public game::IBot<IBoard>
{
    IBoard board;
    long long mcts_iterations;

    UtttBot(const IBoard &board, long long mcts_iterations);

public:
    UtttBot(long long mcts_iterations);
    UtttBot(const UtttBot &other);

    game::IMove MakeMove();

    void SendMove(const game::IMove &move);
};

struct Eval
{
    double operator()(const IBoard *board) const;
};

struct EvalMcts
{
    double operator()(const IBoard *board) const;
};

}