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
void
MCTS_cache_(std::unordered_map<IGame *, GameData *> &cache, const IGame &root_game, GameData *game_data)
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
    for (auto m : moves)
    {
        auto new_game = new IGame(rg);
        auto new_node = new GameData;
        new_game->ApplyMove(m);
        node->children.emplace_back(m, new_node);
        cache[new_game] = new_node;

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
void
MCTS_cache(MCTSNode<IGame> *root)
{
    std::unordered_map<IGame *, GameData *> cache;
    auto game = new IGame(*root->game);
    auto game_data = new GameData;
    cache[game] = game_data;
    for (int i = 0; i < 1000000; ++i)
        MCTS_cache_(cache, *game, game_data);
    root->value = game_data->value;
    root->total = game_data->total;
    for (auto it : game_data->children)
    {
        root->children[it.first] = new MCTSNode<IGame>(game);
        root->children[it.first]->value = it.second->value;
        root->children[it.first]->total = it.second->total;
    }
    for (auto it : cache)
    {
        delete it.first;
        delete it.second;
    }
}