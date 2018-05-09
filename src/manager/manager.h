#pragma once

#include <vector>

#include <game.h>

namespace manager {

class Manager
{
    std::vector<std::pair<game::IBot *, game::IGame *>> bots;

public:
    ~Manager();

    void AddBot(const game::IBot *bot, const game::IGame *game);

    int Match(const game::IBot *bot1, const game::IBot *bot2, const game::IGame *game);
};

}
