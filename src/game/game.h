#pragma once

#include <cstdint>
#include <vector>

namespace game {

typedef int64_t IMove;
typedef int8_t IPlayer;

enum
{
    Undecided = -1,
    Draw = 0,
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

template <typename IGame>
class IBot
{
protected:
    IGame *game;

public:
    IBot(const IGame *game);

    virtual ~IBot();

    virtual IBot *Clone() const = 0;

    virtual IMove MakeMove() = 0;

    virtual void SendMove(const IMove &move);
};

}

#include <game.hpp>