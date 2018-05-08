#pragma once

#include <vector>

typedef long long IMove;

struct IGame
{
    virtual void Move(const IMove &move) = 0;
    virtual std::vector<IMove> GetPossibleMoves() const = 0;
    virtual int GetMoveCount() const;
    virtual IMove GetRandomMove() const;
    virtual int GetStatus() = 0;
    virtual IGame *Clone() const = 0;
    virtual std::size_t Hash() const = 0;
    virtual bool Equal(const IGame *game) const = 0;
    virtual void Print() = 0;
    virtual ~IGame() {}
};
