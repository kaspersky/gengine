#pragma once

#include <vector>
#include <unordered_map>

#include <game.h>

namespace manager {

template <typename IGame>
struct Bot
{
    long long id;
    game::IBot<IGame> *bot;
    double rating;

    Bot(long long id, const game::IBot<IGame> *bot, double rating);
    ~Bot();
};

template <typename IGame>
class Manager
{
    IGame *game;
    std::unordered_map<long long, Bot<IGame> *> bots;

public:
    Manager(const IGame *game);

    ~Manager();

    long long AddBot(const game::IBot<IGame> *bot);

    double GetBotRating(long long bot_id) const;

    std::pair<double, double> Match(long long bot_id1, long long bot_id2, int count);

    void RoundRobin();
};

}

#include "manager.hpp"