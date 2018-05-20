#include <vector>
#include <unordered_map>
#include <cmath>

#include <mcts.h>

template <typename IGame>
void
MCTS_parallel(MCTSNode<IGame> *root)
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
        auto new_node = new MCTSNode<IGame>(node->game);
        new_node->game->ApplyMove(m);
        node->children[m] = new_node;

        nodes.push_back(new_node);

        IGame game(*new_node->game);
        int status = game.GetStatus();
        while (status == game::Undecided)
        {
            auto m = game.GetRandomMove();
            game.ApplyMove(m);
            status = game.GetStatus();
        }

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