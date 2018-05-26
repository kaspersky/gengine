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

template <typename IGame>
std::pair<long long, long long> Count(int depth);

template <typename IGame>
long long CountBFS(int depth);

}

#include <game.hpp>