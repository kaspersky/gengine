#include <cmath>

#include <manager.h>

namespace manager {

static long long g_bot_ids;
static long long g_game_ids;
static double g_default_rating = 1200;
static double g_K = 16.0;

template <typename T>
static int
sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

static std::pair<double, double>
mm(double rating1, double rating2, double result1)
{
    std::pair<double, double> ret = {rating1, rating2};
    double e1 = sgn(rating1 - rating2) * (0.5 - 1.0 / std::pow(2.0, std::abs(rating1 - rating2) / 100.0 + 1)) + 0.5;
    double e2 = 1.0 - e1;
    ret.first += g_K * (result1 - e1);
    ret.second += g_K * (1.0 - result1 - e2);
    return ret;
}

Bot::~Bot()
{
    delete bot;
}

Manager::~Manager()
{
    for (auto it : bots)
        delete it.second;
    for (auto it : games)
        delete it.second;
}

double
Manager::GetBotRating(long long bot_id) const
{
    return bots.find(bot_id)->second->rating;
}

int
Manager::Match(long long bot_id1, long long bot_id2)
{
    auto it1 = bots.find(bot_id1), it2 = bots.find(bot_id2);
    auto game = games.find(it1->second->game_id)->second->Clone();
    auto bot1 = it1->second->bot->Clone(), bot2 = it2->second->bot->Clone();

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

    auto new_ratings = mm(it1->second->rating, it2->second->rating, status == game::IGame::Draw ? 0.5 : (status == 1 ? 1.0 : 0.0));
    it1->second->rating = new_ratings.first, it2->second->rating = new_ratings.second;

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

void
Manager::RoundRobin(long long game_id)
{
    auto bots = bots_by_game.find(game_id)->second;
    for (auto it1 = bots.begin(); it1 != bots.end(); ++it1)
    {
        auto it2 = it1;
        ++it2;
        for (; it2 != bots.end(); ++it2)
            Match(it1->second->id, it2->second->id);
    }
}

}