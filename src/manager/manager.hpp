#include <cmath>

#include "manager.h"

namespace manager {

extern long long g_bot_ids;
extern double g_default_rating;

template <typename IGame>
Bot<IGame>::Bot(long long id, const game::IBot<IGame> *bot, double rating): id(id), bot(bot->Clone()), rating(rating)
{
}

template <typename IGame>
Bot<IGame>::~Bot()
{
    delete bot;
}

template <typename IGame>
Manager<IGame>::Manager(const IGame *game)
{
    this->game = new IGame(*game);
}

template <typename IGame>
Manager<IGame>::~Manager()
{
    for (auto it : bots)
        delete it.second;
    delete game;
}

template <typename IGame>
double
Manager<IGame>::GetBotRating(long long bot_id) const
{
    return bots.find(bot_id)->second->rating;
}

template <typename IGame>
std::pair<double, double>
Manager<IGame>::Match(long long bot_id1, long long bot_id2, int count)
{
    auto it1 = bots.find(bot_id1), it2 = bots.find(bot_id2);

    std::pair<double, double> results;

    for (int i = 0; i < count; ++i)
    {
        auto game_copy = new IGame(*game);
        game::IBot<IGame> *bs[2] = {};
        bs[i % 2] = it1->second->bot->Clone();
        bs[1 - i % 2] = it2->second->bot->Clone();
        int status = game_copy->GetStatus();
        while (status == game::Undecided)
        {
            auto move = bs[0]->MakeMove();
            game_copy->ApplyMove(move);
            bs[1]->SendMove(move);

            status = game_copy->GetStatus();
            if (status != game::Undecided)
                break;

            move = bs[1]->MakeMove();
            game_copy->ApplyMove(move);
            bs[0]->SendMove(move);

            status = game_copy->GetStatus();
        }

        if (status == game::Draw)
            results.first += 0.5, results.second += 0.5;
        else if (status == 1)
            i % 2 == 0 ? results.first += 1.0 : results.second += 1.0;
        else
            i % 2 == 0 ? results.second += 1.0 : results.first += 1.0;

        delete bs[0];
        delete bs[1];
        delete game_copy;
    }

    results.first /= count, results.second /= count;

    auto new_ratings = MM()(it1->second->rating, it2->second->rating, results.first);
    it1->second->rating = new_ratings.first, it2->second->rating = new_ratings.second;

    return results;
}

template <typename IGame>
long long
Manager<IGame>::AddBot(const game::IBot<IGame> *bot)
{
    long long id = g_bot_ids;
    ++g_bot_ids;

    auto m_bot = new Bot<IGame>(id, bot, g_default_rating);
    bots[id] = m_bot;
    return id;
}

template <typename IGame>
void
Manager<IGame>::RoundRobin()
{
    for (auto it1 = bots.begin(); it1 != bots.end(); ++it1)
    {
        auto it2 = it1;
        ++it2;
        for (; it2 != bots.end(); ++it2)
            Match(it1->second->id, it2->second->id, 2);
    }
}

}