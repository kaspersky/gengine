#include <vector>
#include <unordered_map>
#include <cmath>
#include <random>
#include <unordered_set>

#include <mcts.h>

MCTSNode::MCTSNode(const game::IGame *game): game(game->Clone())
{
    value = 0.0;
    total = 0;
}

MCTSNode::MCTSNode(const MCTSNode &other)
{
    game = nullptr;
    if (other.game != nullptr)
        game = other.game->Clone();
    value = other.value;
    total = other.total;

    for (auto it : other.children)
        children[it.first] = new MCTSNode(*it.second);
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
        if (node->game->GetStatus() != game::IGame::Undecided)
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

    if (node->game->GetStatus() == game::IGame::Undecided)
    {
        auto m = node->game->GetRandomMove();
        auto it = node->children.find(m);
        if (it == node->children.end())
        {
            MCTSNode *new_node = new MCTSNode(node->game);
            new_node->game->ApplyMove(m);
            node->children[m] = new_node;
            node = new_node;
        }
        else
            node = it->second;
        nodes.push_back(node);
    }

    game::IGame *game = node->game->Clone();
    int status = game->GetStatus();
    while (status == game::IGame::Undecided)
    {
        auto m = game->GetRandomMove();
        game->ApplyMove(m);
        status = game->GetStatus();
    }
    delete game;

    ++nodes[0]->total;
    for (unsigned i = 1; i < nodes.size(); ++i)
    {
        ++nodes[i]->total;
        if (status == game::IGame::Draw)
            continue;
        if (nodes[i - 1]->game->GetPlayerToMove() == status)
            nodes[i]->value += 1;
        else
            nodes[i]->value -= 1;
    }
}

void
MCTS01(MCTSNode *root)
{
    MCTSNode *node = root;
    std::vector<MCTSNode *> nodes = {root};
    while (true)
    {
        if (node->game->GetStatus() != game::IGame::Undecided)
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

    if (node->game->GetStatus() == game::IGame::Undecided)
    {
        auto m = node->game->GetRandomMove();
        auto it = node->children.find(m);
        if (it == node->children.end())
        {
            MCTSNode *new_node = new MCTSNode(node->game);
            new_node->game->ApplyMove(m);
            node->children[m] = new_node;
            node = new_node;
        }
        else
            node = it->second;
        nodes.push_back(node);
    }

    game::IGame *game = node->game->Clone();
    int status = game->GetStatus();
    while (status == game::IGame::Undecided)
    {
        auto m = game->GetRandomMove();
        game->ApplyMove(m);
        status = game->GetStatus();
    }
    delete game;

    ++nodes[0]->total;
    for (unsigned i = 1; i < nodes.size(); ++i)
    {
        ++nodes[i]->total;
        if (status == game::IGame::Draw)
            nodes[i]->value += 0.5;
        else if (nodes[i - 1]->game->GetPlayerToMove() == status)
            nodes[i]->value += 1.0;
    }
}

static void
CountUnique_(const MCTSNode *root, std::unordered_set<const game::IGame *> &set)
{
    set.insert(root->game);
    for (auto it : root->children)
        CountUnique_(it.second, set);
}

long long
CountUnique(const MCTSNode *root)
{
    std::unordered_set<const game::IGame *> set;
    CountUnique_(root, set);
    return set.size();
}