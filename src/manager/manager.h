#pragma once

#include <vector>
#include <unordered_map>

#include <game.h>

namespace manager {

struct Bot
{
    long long id;
    game::IBot *bot;
    long long game_id;
    double rating;

    ~Bot();
};

class Manager
{
    std::unordered_map<long long, Bot *> bots;
    std::unordered_map<long long, std::unordered_map<long long, Bot *>> bots_by_game;
    std::unordered_map<long long, game::IGame *> games;

public:
    ~Manager();

    long long AddGame(const game::IGame *game);

    long long AddBot(const game::IBot *bot, long long game_id);

    double GetBotRating(long long bot_id) const;

    int Match(long long bot_id1, long long bot_id2);

    void RoundRobin(long long game_id);
};

}