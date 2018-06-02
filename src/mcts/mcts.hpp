#include <cmath>
#include <unordered_set>

#include "mcts.h"

template <typename IGame>
int
RandomPlayout<IGame>::operator()(const IGame &game) const
{
    auto ngame(game);
    int status = ngame.GetStatus();
    while (status == game::Undecided)
    {
        auto m = ngame.GetRandomMove();
        ngame.ApplyMove(m);
        status = ngame.GetStatus();
    }
    return status;
}

template <typename IGame, typename RandomPlayout>
static void
MCTS_(MCTSNode *root, const IGame &root_game)
{
    MCTSNode *node = root;
    auto game = root_game;
    auto player = 3 - root_game.GetPlayerToMove();
    std::vector<MCTSNode *> nodes = {root};
    while (true)
    {
        if (game.GetStatus() != game::Undecided)
            break;
        int s = game.GetMoveCount();
        if (static_cast<int>(node->children.size()) < s)
            break;

        MCTSNode *child = nullptr;
        double min = 0.0;
        game::IMove move = -1;
        for (const auto it : node->children)
        {
            const MCTSNode *n = it.second;
            double p = static_cast<double>(n->value) / n->total + std::sqrt(2.0 * std::log(node->total) / n->total);
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
        auto m = game.GetRandomMove();
        auto it = node->children.find(m);
        if (it == node->children.end())
        {
            auto new_node = new MCTSNode;
            game.ApplyMove(m);
            node->children[m] = new_node;
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
        if (status == game::Draw)
            continue;
        if (player == status)
            nodes[i]->value += 1;
        else
            nodes[i]->value -= 1;
    }
}

template <typename IGame, typename RandomPlayout>
std::vector<MCTSResults>
MCTS<IGame, RandomPlayout>::operator()(const IGame &game, long long iterations) const
{
    MCTSNode root;
    for (long long i = 0; i < iterations; ++i)
        MCTS_<IGame, RandomPlayout>(&root, game);

    std::vector<MCTSResults> results;
    for (auto it : root.children)
        results.emplace_back(it.first, it.second->value, it.second->total);
    return results;
}

template <typename IGame, typename RandomPlayout>
static void
MCTS01_(MCTSNode *root, const IGame &root_game)
{
    MCTSNode *node = root;
    auto game = root_game;
    auto player = root_game.GetPlayerToMove();
    std::vector<MCTSNode *> nodes = {root};
    while (true)
    {
        if (game.GetStatus() != game::Undecided)
            break;
        int s = game.GetMoveCount();
        if (static_cast<int>(node->children.size()) < s)
            break;

        MCTSNode *child = nullptr;
        double min = 0.0;
        game::IMove move = -1;
        for (const auto it : node->children)
        {
            const MCTSNode *n = it.second;
            double p = static_cast<double>(n->value) / n->total + std::sqrt(2.0 * std::log(node->total) / n->total);
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
        auto m = game.GetRandomMove();
        auto it = node->children.find(m);
        if (it == node->children.end())
        {
            auto new_node = new MCTSNode;
            game.ApplyMove(m);
            node->children[m] = new_node;
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
        if (status == game::Draw)
            nodes[i]->value += 0.5;
        else if (player == status)
            nodes[i]->value += 1.0;
    }
}

template <typename IGame, typename RandomPlayout>
std::vector<MCTSResults>
MCTS01<IGame, RandomPlayout>::operator()(const IGame &game, long long iterations) const
{
    MCTSNode root;
    for (long long i = 0; i < iterations; ++i)
        MCTS01_<IGame, RandomPlayout>(&root, game);

    std::vector<MCTSResults> results;
    for (auto it : root.children)
        results.emplace_back(it.first, it.second->value, it.second->total);
    return results;
}

template <typename IGame>
static void
CountUnique_(const MCTSNode &root, const IGame &game, std::unordered_set<IGame> &set)
{
    set.insert(game);
    for (const auto &it : root.children)
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