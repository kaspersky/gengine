#include <vector>
#include <unordered_map>
#include <cmath>
#include <future>
#include <thread>

#include "mcts.h"

template <typename IGame, typename RandomPlayout>
static void
MCTS_parallel_(MCTSNode *root, const IGame &root_game)
{
    auto node = root;
    auto game = root_game;
    auto player = 3 - root_game.GetPlayerToMove();
    std::vector<MCTSNode *> nodes = {root};
    while (!node->children.empty())
    {
        MCTSNode *child = nullptr;
        double min = 0.0;
        game::IMove move = -1;
        for (auto n : node->children)
        {
            double p = static_cast<double>(n->value) / n->total + std::sqrt(2.0 * std::log(node->total) / n->total);
            if (p > min || child == nullptr)
            {
                min = p;
                move = n->move;
                child = n;
            }
        }
        node = child;
        nodes.push_back(node);
        game.ApplyMove(move);
    }

    auto status = game.GetStatus();
    if (status != game::Undecided)
    {
        auto p = player;
        for (unsigned i = 0; i < nodes.size(); ++i, p = 3 - p)
        {
            ++nodes[i]->total;
            if (status == game::Draw)
                continue;
            if (p == status)
                nodes[i]->value += 1;
            else
                nodes[i]->value -= 1;
        }
        return;
    }

    auto moves = game.GetPossibleMoves();
    for (auto m : moves)
    {
        auto new_node = new MCTSNode(m);
        auto new_game = game;
        new_game.ApplyMove(m);
        node->children.push_back(new_node);
        nodes.push_back(new_node);
        auto status = RandomPlayout()(new_game);
        auto p = player;
        for (unsigned i = 0; i < nodes.size(); ++i, p = 3 - p)
        {
            ++nodes[i]->total;
            if (status == game::Draw)
                continue;
            if (p == status)
                nodes[i]->value += 1;
            else
                nodes[i]->value -= 1;
        }
        nodes.pop_back();
    }
}

template <typename IGame, typename RandomPlayout>
std::vector<MCTSResults>
MCTS_parallel<IGame, RandomPlayout>::operator()(const IGame &game, long long iterations) const
{
    std::vector<std::future<std::vector<MCTSResults>>> futures;
    int num_threads = std::thread::hardware_concurrency();
    for (int i = 0; i < num_threads; ++i)
        futures.emplace_back(std::async(std::launch::async, [game, iterations] {
            MCTSNode root(-1);
            for (int i = 0; i < iterations; ++i)
                MCTS_parallel_<IGame, RandomPlayout>(&root, game);
            std::vector<MCTSResults> results;
            for (auto it : root.children)
                results.emplace_back(it->move, it->value, it->total);
            return results;
        }));

    std::unordered_map<game::IMove, std::pair<double, long long>> um_results;
    for (auto &f : futures)
    {
        auto result = f.get();
        for (const auto &r : result)
        {
            auto &p = um_results[r.move];
            p.first += r.value, p.second += r.total;
        }
    }

    std::vector<MCTSResults> results;
    for (auto it : um_results)
        results.emplace_back(it.first, it.second.first, it.second.second);
    return results;
}