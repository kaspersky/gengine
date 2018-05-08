#pragma once

#include <vector>
#include <array>

#include <game.h>

namespace uttt {

enum BoardStatus
{
    Undecided = 0,
    Win1 = 1,
    Win2 = 2,
    Draw = 3,
};

struct Board
{
    int macro;
    std::array<short, 9> micro;
    char next;

    Board(int macro, const std::array<short, 9> &micro, char next);

    Board();

    bool operator==(const Board &other) const;

    void print() const;

    void computeMacro();

    void getMoves(std::vector<char> &moves) const;

    void applyMove(char move, char player);

    void updateMicro(const std::array<char, 81> &other);

    char getStatus();
};

class IBoard: public IGame
{
    struct Board board;
    char player;

public:
    IBoard();
    void Move(const IMove &move);
    std::vector<IMove> GetPossibleMoves() const;
    int GetStatus();
    IGame *Clone() const;
    std::size_t Hash() const;
    bool Equal(const IGame *game) const;
    void Print();
};

}
