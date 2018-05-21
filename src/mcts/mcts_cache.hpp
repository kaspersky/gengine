#include <vector>
#include <unordered_map>
#include <cmath>

#include <mcts.h>

struct GameData
{
    double value;
    long long total;
    std::vector<std::pair<game::IMove, GameData *>> children;

    GameData(): value(0.0), total(0)
    {
    }
};

template <typename IGame>
struct GameHash
{
    std::size_t operator()(IGame *game) const
    {
        return game->Hash();
    }
};

template <typename IGame>
struct GameEqual
{
    bool operator()(IGame *game1, IGame *game2) const
    {
        return *game1 == *game2;
    }
};

template <typename IGame>
void
MCTS_cache_(std::unordered_map<IGame *, GameData *, GameHash<IGame>, GameEqual<IGame>> &cache, const IGame &root_game, GameData *game_data)
{
    IGame rg(root_game);
    game::IPlayer player_to_move = rg.GetPlayerToMove();
    auto node = game_data;
    std::vector<GameData *> nodes = {node};
    while (!node->children.empty())
    {
        GameData *child = nullptr;
        game::IMove move = -1;
        double min = 0.0;
        for (auto it : node->children)
        {
            auto n = it.second;
            double p = static_cast<double>(n->value) / n->total + std::sqrt(2.0 * std::log(node->total) / n->total);
            if (p > min || child == nullptr)
            {
                min = p;
                child = it.second;
                move = it.first;
            }
        }
        node = child;
        rg.ApplyMove(move);
        nodes.push_back(node);
    }

    game::IPlayer player = player_to_move;
    int status = rg.GetStatus();
    if (status != game::Undecided)
    {
        ++nodes[0]->total;
        for (unsigned i = 1; i < nodes.size(); ++i, player = 3 - player)
        {
            ++nodes[i]->total;
            if (status == game::Draw)
                continue;
            if (player == status)
                nodes[i]->value += 1;
            else
                nodes[i]->value -= 1;
        }
        return;
    }

    auto moves = rg.GetPossibleMoves();
    std::unordered_map<IGame *, game::IMove, GameHash<IGame>, GameEqual<IGame>> positions;
    for (auto m : moves)
    {
        auto new_game = new IGame(rg);
        new_game->ApplyMove(m);
        if (positions.find(new_game) != positions.end())
        {
            delete new_game;
            continue;
        }
        positions[new_game] = m;
    }
    for (auto p : positions)
    {
        GameData *new_node = nullptr;
        auto it = cache.find(p.first);
        if (it == cache.end())
        {
            new_node = new GameData;
            cache[p.first] = new_node;
        }
        else
        {
            new_node = it->second;
            delete p.first;
        }

        node->children.emplace_back(p.second, new_node);

        nodes.push_back(new_node);

        IGame game(rg);
        int status = game.GetStatus();
        while (status == game::Undecided)
        {
            auto m = game.GetRandomMove();
            game.ApplyMove(m);
            status = game.GetStatus();
        }

        player = player_to_move;
        ++nodes[0]->total;
        for (unsigned i = 1; i < nodes.size(); ++i, player = 3 - player)
        {
            ++nodes[i]->total;
            if (status == game::Draw)
                continue;
            if (player == status)
                nodes[i]->value += 1;
            else
                nodes[i]->value -= 1;
        }

        nodes.pop_back();
    }
}

template <typename IGame>
std::vector<std::pair<game::IMove, std::pair<double, long long>>>
MCTS_cache(const IGame &game, long long iterations)
{
    std::unordered_map<IGame *, GameData *, GameHash<IGame>, GameEqual<IGame>> cache;
    auto game_copy = new IGame(game);
    auto game_data = new GameData;
    cache[game_copy] = game_data;
    for (long long i = 0; i < iterations; ++i)
        MCTS_cache_(cache, *game_copy, game_data);
    std::vector<std::pair<game::IMove, std::pair<double, long long>>> results;
    for (auto it : game_data->children)
        results.push_back({it.first, {it.second->value, it.second->total}});
    for (auto it : cache)
    {
        delete it.first;
        delete it.second;
    }
    return results;
}