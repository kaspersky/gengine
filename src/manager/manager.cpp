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
Match(const game::IBot *bot1, const game::IBot *bot2, const game::IGame *game)
{
    auto *b1 = bot1->Clone(), *b2 = bot2->Clone();
    auto g = game->Clone();

    b1->Init(g), b2->Init(g);

    int status = game::IGame::Undecided;

    while ((status = g->GetStatus()) == game::IGame::Undecided)
    {
        auto move = b1->MakeMove();
        g->ApplyMove(move);
        b2->SendMove(move);

        if ((status = g->GetStatus()) != game::IGame::Undecided)
            break;

        move = b2->MakeMove();
        g->ApplyMove(move);
        b1->SendMove(move);
    }

    delete b1;
    delete b2;
    delete g;

    return status;
}

void
Manager::AddBot(const game::IBot *bot, const game::IGame *game)
{
    bots.emplace_back(bot->Clone(), game->Clone());
}

}
