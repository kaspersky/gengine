#include <iostream>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <random>

#include <mcts.h>

MCTSNode::MCTSNode()
{
    game = nullptr;
    value = 0.0;
    total = 0;
}

MCTSNode::~MCTSNode()
{
    delete game;
    for (auto it : children)
        delete it.second;
}

void
MCTS(MCTSNode *root)
{
    MCTSNode *node = root;
    std::vector<MCTSNode *> nodes = {root};
    while (true)
    {
        if (node->game->GetStatus() != 0)
            break;
        int s = node->game->GetMoveCount();
        if (static_cast<int>(node->children.size()) < s)
            break;

        MCTSNode *child = nullptr;
        double min = 0.0;
        for (auto it : node->children)
        {
            const MCTSNode *n = it.second;
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

    if (node->game->GetStatus() == 0)
    {
        auto m = node->game->GetRandomMove();
        auto it = node->children.find(m);
        if (it == node->children.end())
        {
            MCTSNode *new_node = new MCTSNode();
            new_node->game = node->game->Clone();
            new_node->game->Move(m);
            node->children[m] = new_node;
            node = new_node;
        }
        else
            node = it->second;
        nodes.push_back(node);
    }

    IGame *game = node->game->Clone();
    int status = game->GetStatus();
    while (status == 0)
    {
        auto m = game->GetRandomMove();
        game->Move(m);
        status = game->GetStatus();
    }
    delete game;

    for (unsigned i = 0; i < nodes.size(); i++)
    {
        nodes[i]->total++;

        if (status == 1)
        {
            if (i % 2 == 0)
                nodes[i]->value -= 1;
            else
                nodes[i]->value += 1;
        }
        else if (status == 2)
        {
            if (i % 2 == 0)
                nodes[i]->value += 1;
            else
                nodes[i]->value -= 1;
        }
    }
}
