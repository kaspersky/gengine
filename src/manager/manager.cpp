#include <manager.h>

namespace manager {

static long long g_bot_ids;
static long long g_game_ids;
static double g_default_rating = 1200;

Bot::~Bot()
{
    delete bot;
}

Manager::~Manager()
{
    for (auto it : bots)
        delete it.second;
}

int
Manager::Match(long long bot_id1, long long bot_id2)
{
    auto it1 = bots.find(bot_id1), it2 = bots.find(bot_id2);
    auto game = games.find(it1->second->game_id)->second;
    auto bot1 = it1->second->bot->Clone(), bot2 = it2->second->bot->Clone();
    bot1->Init(game), bot2->Init(game);

    int status = game->GetStatus();
    while (status == game::IGame::Undecided)
    {
        auto move = bot1->MakeMove();
        game->ApplyMove(move);
        bot2->SendMove(move);

        status = game->GetStatus();
        if (status != game::IGame::Undecided)
            break;

        move = bot2->MakeMove();
        game->ApplyMove(move);
        bot1->SendMove(move);

        status = game->GetStatus();
    }

    delete bot1;
    delete bot2;
    delete game;

    return status;
}

long long
Manager::AddGame(const game::IGame *game)
{
    long long id = g_game_ids;
    ++g_game_ids;
    games[id] = game->Clone();
    return id;
}

long long
Manager::AddBot(const game::IBot *bot, long long game_id)
{
    long long id = g_bot_ids;
    ++g_bot_ids;

    Bot *m_bot = new Bot{id, bot->Clone(), game_id, g_default_rating};
    bots[id] = m_bot;
    bots_by_game[game_id][id] = m_bot;
    return id;
}

}
