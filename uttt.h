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
    char player;

public:
    IBoard(int macro, const std::array<short, 9> &micro, char next, char player);

    IBoard();

    bool operator==(const IBoard &other) const;

    void computeMacro();

    std::vector<game::IMove> GetPossibleMoves() const;

    void ApplyMove(const game::IMove &move);

    void updateMicro(const std::array<char, 81> &other);

    int GetStatus() const;

    IGame *Clone() const;

    std::size_t Hash() const;

    bool Equal(const game::IGame *game) const;

    void Print() const;
};

}
