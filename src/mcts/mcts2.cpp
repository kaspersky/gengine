#include <vector>
#include <unordered_map>
#include <cmath>

#include <mcts.h>

void
MCTS2(MCTSNode *root)
{
    MCTSNode *node = root;
    std::vector<MCTSNode *> nodes = {root};
    while (!node->children.empty())
    {
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

    int status = node->game->GetStatus();
    if (status == 0)
    {
        auto moves = node->game->GetPossibleMoves();
        for (auto m : moves)
        {
            MCTSNode *new_node = new MCTSNode;
            new_node->game = node->game->Clone();
            new_node->game->ApplyMove(m);
            node->children[m] = new_node;

            nodes.push_back(new_node);

            game::IGame *game = new_node->game->Clone();
            int status = game->GetStatus();
            while (status == game::IGame::Undecided)
            {
                auto m = game->GetRandomMove();
                game->ApplyMove(m);
                status = game->GetStatus();
            }
            delete game;

            for (unsigned i = 0; i < nodes.size(); ++i)
                ++nodes[i]->total;

            if (status != 3)
            {
                int s1 = -1, s2 = 1;
                if (status == 2)
                    s1 = 1, s2 = -1;
                for (unsigned i = 0; i < nodes.size(); i += 2)
                    nodes[i]->value += s1;
                for (unsigned i = 1; i < nodes.size(); i += 2)
                    nodes[i]->value += s2;
            }

            nodes.pop_back();
        }
        return;
    }

    for (unsigned i = 0; i < nodes.size(); ++i)
        ++nodes[i]->total;

    if (status != game::IGame::Draw)
    {
        int s1 = -1, s2 = 1;
        if (status == 2)
            s1 = 1, s2 = -1;
        for (unsigned i = 0; i < nodes.size(); i += 2)
            nodes[i]->value += s1;
        for (unsigned i = 1; i < nodes.size(); i += 2)
            nodes[i]->value += s2;
    }
}