#include <unordered_set>

#include "game.h"

namespace game {

template <typename IGame>
IBot<IGame>::IBot(const IGame *game)
{
    if (game != nullptr)
        this->game = new IGame(*game);
}

template <typename IGame>
IBot<IGame>::~IBot()
{
    delete game;
}

template <typename IGame>
void
IBot<IGame>::SendMove(const IMove &move)
{
    game->ApplyMove(move);
}

template <typename IGame>
static long long
CountUnique_(const IGame &game, int depth, std::unordered_set<IGame> &set)
{
    set.insert(game);

    long long count = 1;

    if (depth == 0 || game.GetStatus() != Undecided)
        return count;

    auto moves = game.GetPossibleMoves();
    for (auto m : moves)
    {
        auto game_copy(game);
        game_copy.ApplyMove(m);
        count += CountUnique_(game_copy, depth - 1, set);
    }
    return count;
}

template <typename IGame>
std::pair<long long, long long>
CountUnique(int depth)
{
    std::unordered_set<IGame> set;
    long long total = CountUnique_({}, depth, set);
    std::pair<long long, long long> result = {set.size(), total};
    return result;
}

template <typename IGame>
static long long
Count_(const IGame &game, int depth)
{
    long long count = 1;

    if (depth == 0 || game.GetStatus() != Undecided)
        return count;

    auto moves = game.GetPossibleMoves();
    for (auto m : moves)
    {
        auto game_copy(game);
        game_copy.ApplyMove(m);
        count += Count_(game_copy, depth - 1);
    }
    return count;
}

template <typename IGame>
long long
Count(int depth)
{
    return Count_(IGame(), depth);
}

template <typename IGame>
long long
CountUniqueBFS(int depth)
{
    std::unordered_set<IGame> total, set, next_set;
    set.insert({{}}), total.insert({{}});
    for (int i = 0; i < depth; ++i)
    {
        next_set.clear();
        for (auto g : set)
        {
            if (g.GetStatus() != Undecided)
                continue;
            auto moves = g.GetPossibleMoves();
            for (auto m : moves)
            {
                auto ng = g;
                ng.ApplyMove(m);
                next_set.insert(ng);
                total.insert(ng);
            }
        }
        set = next_set;
    }
    return total.size();
}

template <typename IGame>
long long
CountBFS(int depth)
{
    long long total = 1;
    std::vector<IGame> levels[2] = {{{}}};
    int current_level = 0;
    for (int i = 0; i < depth; ++i)
    {
        levels[1 - current_level].clear();
        for (const auto &g : levels[current_level])
        {
            if (g.GetStatus() != Undecided)
                continue;
            auto moves = g.GetPossibleMoves();
            for (auto m : moves)
            {
                auto ng = g;
                ng.ApplyMove(m);
                levels[1 - current_level].emplace_back(ng);
                ++total;
            }
        }
        current_level = 1 - current_level;
    }
    return total;
}

}