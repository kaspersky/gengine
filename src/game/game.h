#pragma once

#include <cstdint>
#include <vector>

namespace game {

typedef uint64_t IMove;
typedef int8_t IPlayer;

class IGame
{
public:
    enum
    {
        Undecided = -1,
        Draw = 0,
    };

    virtual ~IGame();

    virtual void ApplyMove(const IMove &move) = 0;

    virtual std::vector<IMove> GetPossibleMoves() const = 0;

    virtual int GetMoveCount() const;

    virtual IMove GetRandomMove() const;

    virtual int GetStatus() const = 0;

    virtual IPlayer GetPlayerToMove() const = 0;

    virtual IGame *Clone() const = 0;

    virtual std::size_t Hash() const = 0;

    virtual bool Equal(const IGame *game) const = 0;

    virtual void Print() const = 0;
};

class IBot
{
protected:
    IGame *game;

public:
    IBot(const IGame *game);

    virtual ~IBot();

    virtual IMove MakeMove() = 0;

    virtual void SendMove(const IMove &move);

    virtual IBot *Clone() const = 0;
};

}