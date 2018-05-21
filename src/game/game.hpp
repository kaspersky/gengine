#include <unordered_set>

#include <game.h>

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
Count_(const IGame &game, int depth, std::unordered_set<IGame> &set)
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
        count += Count_(game_copy, depth - 1, set);
    }
    return count;
}

template <typename IGame>
std::pair<long long, long long>
Count(int depth)
{
    std::unordered_set<IGame> set;
    long long total = Count_({}, depth, set);
    std::pair<long long, long long> result = {set.size(), total};
    return result;
}

}