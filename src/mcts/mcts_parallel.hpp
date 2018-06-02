#include <vector>
#include <unordered_map>
#include <cmath>
#include <future>
#include <thread>

#include "mcts.h"

struct MCTSParallelNode
{
    game::IMove move;
    double value;
    int64_t total;
    int32_t children_count;
    MCTSParallelNode *children;

    MCTSParallelNode(): move(-1), value(0.0), total(0), children_count(0), children(nullptr)
    {
    }

    void CopyChildren(const MCTSParallelNode &other)
    {
        if (children_count == 0)
        {
            children = nullptr;
            return;
        }
        children = new MCTSParallelNode[children_count];
        for (int i = 0; i < children_count; ++i)
            children[i].CopyChildren(other.children[i]);
    }

    MCTSParallelNode(const MCTSParallelNode &other): move(other.move), value(other.value), total(other.total), children_count(other.children_count)
    {
        CopyChildren(other);
    }

    ~MCTSParallelNode()
    {
        delete [] children;
    }
};

template <typename IGame, typename RandomPlayout>
static void
MCTS_parallel_(MCTSParallelNode *root, const IGame &root_game)
{
    auto node = root;
    auto game = root_game;
    auto player = 3 - root_game.GetPlayerToMove();
    std::vector<MCTSParallelNode *> nodes = {root};
    while (node->children_count != 0)
    {
        MCTSParallelNode *child = nullptr;
        double min = 0.0;
        game::IMove move = -1;
        for (int i = 0; i < node->children_count; ++i)
        {
            auto n = &node->children[i];
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
    node->children_count = moves.size();
    node->children = new MCTSParallelNode[moves.size()];
    for (unsigned i = 0; i < moves.size(); ++i)
    {
        auto new_game = game;
        new_game.ApplyMove(moves[i]);
        node->children[i].move = moves[i];
        nodes.push_back(&node->children[i]);
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
            MCTSParallelNode root;
            root.move = -1;
            for (int i = 0; i < iterations; ++i)
                MCTS_parallel_<IGame, RandomPlayout>(&root, game);
            std::vector<MCTSResults> results;
            for (int k = 0; k < root.children_count; ++k)
                results.emplace_back(root.children[k].move, root.children[k].value, root.children[k].total);
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