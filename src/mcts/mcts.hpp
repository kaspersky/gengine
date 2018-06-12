#include <cmath>
#include <unordered_set>
#include <future>
#include <random>

#include "mcts.h"

namespace mcts {

template <typename IGame>
int
RandomPlayout<IGame>::operator()(const IGame &game) const
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    auto ngame(game);
    int status = ngame.GetStatus();
    while (status == game::Undecided)
    {
        auto moves = ngame.GetPossibleMoves();
        ngame.ApplyMove(moves[std::uniform_int_distribution<>(0, moves.size() - 1)(gen)]);
        status = ngame.GetStatus();
    }
    return status;
}

template <typename IGame, typename RandomPlayout>
static void
MCTS_(MCTSNode *root, const IGame &root_game, std::mt19937 &gen)
{
    MCTSNode *node = root;
    auto game = root_game;
    auto player = 3 - root_game.GetPlayerToMove();
    std::vector<MCTSNode *> nodes = {root};
    while (true)
    {
        if (game.GetStatus() != game::Undecided)
            break;
        if (int(node->children.size()) < node->move_count)
            break;

        MCTSNode *child = nullptr;
        double min = 0.0;
        game::IMove move = -1;
        for (const auto &it : node->children)
        {
            double p = static_cast<double>(it.second->value) / it.second->total + std::sqrt(2.0 * std::log(node->total) / it.second->total);
            if (p > min || child == nullptr)
            {
                min = p;
                move = it.first;
                child = it.second;
            }
        }
        node = child;
        nodes.push_back(node);
        game.ApplyMove(move);
    }

    if (game.GetStatus() == game::Undecided)
    {
        auto moves = game.GetPossibleMoves();
        auto m = moves[std::uniform_int_distribution<>(0, moves.size() - 1)(gen)];
        auto it = node->children.find(m);
        if (it == node->children.end())
        {
            game.ApplyMove(m);
            auto new_node = new MCTSNode(game.GetPossibleMoves().size());
            node->children.emplace(m, new_node);
            node = new_node;
        }
        else
            node = it->second;
        nodes.push_back(node);
    }

    int status = RandomPlayout()(game);

    for (unsigned i = 0; i < nodes.size(); ++i, player = 3 - player)
    {
        ++nodes[i]->total;
        if (player == status)
            nodes[i]->value += 1.0;
        else if (status == game::Draw)
            nodes[i]->value += 0.5;
    }
}

template <typename IGame, typename RandomPlayout>
std::vector<MCTSResults>
MCTS(const IGame &game, long long iterations)
{
    std::vector<std::future<std::vector<MCTSResults>>> futures;
    int num_threads = std::thread::hardware_concurrency();
    for (int i = 0; i < num_threads; ++i)
        futures.emplace_back(std::async(std::launch::async, [game, iterations] {
            std::random_device rd;
            std::mt19937 gen(rd());

            MCTSNode root(game.GetPossibleMoves().size());
            for (int i = 0; i < iterations; ++i)
                MCTS_<IGame, RandomPlayout>(&root, game, gen);
            std::vector<MCTSResults> results;
            for (const auto &it : root.children)
                results.emplace_back(it.first, it.second->value, it.second->total);
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

template <typename IGame>
static void
CountUnique_(const MCTSNode &root, const IGame &game, std::unordered_set<IGame> &set)
{
    set.insert(game);
    for (auto it : root.children)
    {
        auto ng = game;
        ng.ApplyMove(it.first);
        CountUnique_(it.second, ng, set);
    }
}

template <typename IGame>
long long
CountUnique(const MCTSNode &root, const IGame &game)
{
    std::unordered_set<IGame> set;
    CountUnique_<IGame>(root, game, set);
    return set.size();
}

}