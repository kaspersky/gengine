#pragma once

#include <vector>

namespace game {

typedef long long IMove;

class IGame
{
public:
    enum Status
    {
        Undecided = 0,
        Win1 = 1,
        Win2 = 2,
        Draw = 3,
    };

    virtual void ApplyMove(const IMove &move) = 0;
    virtual std::vector<IMove> GetPossibleMoves() const = 0;
    virtual int GetMoveCount() const;
    virtual IMove GetRandomMove() const;
    virtual int GetStatus() const = 0;
    virtual IGame *Clone() const = 0;
    virtual std::size_t Hash() const = 0;
    virtual bool Equal(const IGame *game) const = 0;
    virtual void Print() const = 0;
    virtual ~IGame();
};

class IBot
{
    IGame *game;

public:
    IBot();

    virtual void Init(const IGame *game) = 0;

    virtual IMove MakeMove() = 0;

    virtual void SendMove(const IMove &move) = 0;

    virtual IBot *Clone() const = 0;

    virtual ~IBot();
};

}
