#pragma once

#include <array>
#include <vector>

#include <game.h>

namespace ttt {

typedef std::array<std::array<uint8_t, 3>, 3> TBoard;

class Board
{
    TBoard board;
    game::IPlayer player;

public:
    Board();

    bool operator==(const Board &other) const;

    int GetStatus() const;

    void ApplyMove(const game::IMove &move);

    std::vector<game::IMove> GetPossibleMoves() const;

    int GetMoveCount() const;

    game::IPlayer GetPlayerToMove() const;

    std::size_t Hash() const;

    void Print() const;
};

}

namespace std {
    template <>
    struct hash<ttt::Board>
    {
        std::size_t operator()(const ttt::Board &board) const
        {
            return board.Hash();
        }
    };
}