#include <vector>
#include <unordered_set>
#include <cmath>

#include <mcts.h>

template <typename IGame>
struct GameData
{
    IGame *game;
    double value;
    long long total;
    std::vector<std::pair<game::IMove, GameData *>> children;

    GameData(IGame *game): game(game), value(0.0), total(0)
    {
    }

    ~GameData()
    {
        delete game;
    }
};

template <typename IGame>
struct GameDataHash
{
    std::size_t operator()(GameData<IGame> *game_data) const
    {
        return game_data->game->Hash();
    }
};

template <typename IGame>
struct GameDataEqual
{
    bool operator()(const GameData<IGame> *game_data1, const GameData<IGame> *game_data2) const
    {
        return *game_data1->game == *game_data2->game;
    }
};

template <typename IGame>
void
MCTS_cache_(std::unordered_set<GameData<IGame> *, GameDataHash<IGame>, GameDataEqual<IGame>> &cache, GameData<IGame> *root_game_data)
{
    game::IPlayer player_to_move = root_game_data->game->GetPlayerToMove();
    auto node = root_game_data;
    std::vector<GameData<IGame> *> nodes = {node};
    while (!node->children.empty())
    {
        GameData<IGame> *child = nullptr;
        double min = 0.0;
        for (auto it : node->children)
        {
            auto n = it.second;
            double p = static_cast<double>(n->value) / n->total + std::sqrt(2.0 * std::log(node->total) / n->total);
            if (p > min || child == nullptr)
            {
                min = p;
                child = it.second;
            }
        }
        node = child;
        nodes.push_back(node);
    }

    game::IPlayer player = player_to_move;
    int status = node->game->GetStatus();
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

    auto moves = node->game->GetPossibleMoves();
    std::unordered_map<GameData<IGame> *, game::IMove, GameDataHash<IGame>, GameDataEqual<IGame>> positions;
    for (auto m : moves)
    {
        auto new_game_data = new GameData<IGame>(new IGame(*node->game));
        new_game_data->game->ApplyMove(m);
        if (positions.find(new_game_data) != positions.end())
        {
            delete new_game_data;
            continue;
        }
        positions[new_game_data] = m;
    }
    for (auto p : positions)
    {
        GameData<IGame> *new_node = nullptr;
        auto it = cache.find(p.first);
        if (it == cache.end())
        {
            new_node = p.first;
            cache.insert(new_node);
        }
        else
        {
            new_node = *it;
            delete p.first;
        }

        node->children.emplace_back(p.second, new_node);

        nodes.push_back(new_node);

        IGame game(*new_node->game);
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
    std::unordered_set<GameData<IGame> *, GameDataHash<IGame>, GameDataEqual<IGame>> cache;
    auto game_data = new GameData<IGame>(new IGame(game));
    cache.insert(game_data);
    for (long long i = 0; i < iterations; ++i)
        MCTS_cache_(cache, game_data);
    std::vector<std::pair<game::IMove, std::pair<double, long long>>> results;
    for (auto it : game_data->children)
        results.push_back({it.first, {it.second->value, it.second->total}});
    for (auto it : cache)
        delete it;
    return results;
}