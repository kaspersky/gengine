#include <manager.h>

namespace manager {

Manager::~Manager()
{
    for (auto bot : bots)
    {
        delete bot.first;
        delete bot.second;
    }
}

int
Manager::Match(const game::IBot *bot1, const game::IBot *bot2, const game::IGame *game)
{
    auto *b1 = bot1->Clone(), *b2 = bot2->Clone();
    game::IGame *g = game->Clone();

    b1->Init(g), b2->Init(g);

    int status = g->GetStatus();

    while (status == game::IGame::Undecided)
    {
        auto move = b1->MakeMove();
        g->ApplyMove(move);
        b2->SendMove(move);

        status = g->GetStatus();
        if (status != game::IGame::Undecided)
            break;

        move = b2->MakeMove();
        g->ApplyMove(move);
        b1->SendMove(move);

        status = g->GetStatus();
    }

    delete b1;
    delete b2;
    delete g;

    return status;
}

void
Manager::AddBot(const game::IBot *bot, const game::IGame *game)
{
    game::IGame *g = nullptr;
    if (game != nullptr)
        g = game->Clone();
    bots.emplace_back(bot->Clone(), g);
}

}
