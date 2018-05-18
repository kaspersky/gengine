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

    int GetStatus() const;

    void ApplyMove(const game::IMove &move);

    game::IMove GetRandomMove() const;

    std::vector<game::IMove> GetPossibleMoves() const;

    int GetMoveCount() const;

    game::IPlayer GetPlayerToMove() const;

    void Print() const;
};

#if 0
    class IGame
    {
    public:
        virtual ~IGame();

        virtual void ApplyMove(const IMove &move) = 0;

        virtual std::vector<IMove> GetPossibleMoves() const = 0;

        virtual int GetMoveCount() const;

        virtual IMove GetRandomMove() const;

        virtual int GetStatus() const = 0;

        virtual IPlayer GetPlayerToMove() const = 0;

        virtual std::size_t Hash() const = 0;

        virtual bool Equal(const IGame *game) const = 0;

        virtual void Print() const = 0;
    };
#endif


}
