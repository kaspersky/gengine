#include <vector>
#include <unordered_map>
#include <cmath>

#include "mcts.h"

template <typename IGame, typename RandomPlayout>
void
MCTS_parallel_(MCTSNode<IGame> *root)
{
    MCTSNode<IGame> *node = root;
    std::vector<MCTSNode<IGame> *> nodes = {root};
    while (!node->children.empty())
    {
        MCTSNode<IGame> *child = nullptr;
        double min = 0.0;
        for (auto it : node->children)
        {
            const MCTSNode<IGame> *n = it.second;
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

    int status = node->game->GetStatus();
    if (status != game::Undecided)
    {
        ++nodes[0]->total;
        for (unsigned i = 1; i < nodes.size(); ++i)
        {
            ++nodes[i]->total;
            if (status == game::Draw)
                continue;
            if (nodes[i - 1]->game->GetPlayerToMove() == status)
                nodes[i]->value += 1;
            else
                nodes[i]->value -= 1;
        }
        return;
    }

    auto moves = node->game->GetPossibleMoves();
    for (auto m : moves)
    {
        auto new_node = new MCTSNode<IGame>(*node->game);
        new_node->game->ApplyMove(m);
        node->children[m] = new_node;

        nodes.push_back(new_node);

        int status = RandomPlayout()(*new_node->game);

        ++nodes[0]->total;
        for (unsigned i = 1; i < nodes.size(); ++i)
        {
            ++nodes[i]->total;
            if (status == game::Draw)
                continue;
            if (nodes[i - 1]->game->GetPlayerToMove() == status)
                nodes[i]->value += 1;
            else
                nodes[i]->value -= 1;
        }

        nodes.pop_back();
    }
}

template <typename IGame, typename RandomPlayout=RandomPlayout<IGame>>
std::vector<std::pair<game::IMove, std::pair<double, long long>>>
MCTS_parallel(const IGame &game, long long iterations)
{
    MCTSNode<IGame> root(game);
    for (long long i = 0; i < iterations; ++i)
        MCTS_parallel_<IGame, RandomPlayout>(&root);

    std::vector<std::pair<game::IMove, std::pair<double, long long>>> results;
    for (auto it : root.children)
        results.emplace_back(it.first, std::pair<double, long long>{it.second->value, it.second->total});
    return results;
}