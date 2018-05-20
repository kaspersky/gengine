#include <cmath>
#include <unordered_set>

#include <mcts.h>

template <typename IGame>
MCTSNode<IGame>::MCTSNode(const IGame &game): game(new IGame(game)), value(0.0), total(0)
{
}

template <typename IGame>
MCTSNode<IGame>::MCTSNode(const MCTSNode &other): game(new IGame(other.game)), value(other.value), total(other.total)
{
    for (auto it : other.children)
        children[it.first] = new MCTSNode(*it.second);
}

template <typename IGame>
MCTSNode<IGame>::~MCTSNode()
{
    delete game;
    for (auto it : children)
        delete it.second;
}

template <typename IGame>
void
MCTS_(MCTSNode<IGame> *root)
{
    MCTSNode<IGame> *node = root;
    std::vector<MCTSNode<IGame> *> nodes = {root};
    while (true)
    {
        if (node->game->GetStatus() != game::Undecided)
            break;
        int s = node->game->GetMoveCount();
        if (static_cast<int>(node->children.size()) < s)
            break;

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

    if (node->game->GetStatus() == game::Undecided)
    {
        auto m = node->game->GetRandomMove();
        auto it = node->children.find(m);
        if (it == node->children.end())
        {
            MCTSNode<IGame> *new_node = new MCTSNode<IGame>(*node->game);
            new_node->game->ApplyMove(m);
            node->children[m] = new_node;
            node = new_node;
        }
        else
            node = it->second;
        nodes.push_back(node);
    }

    IGame *game = new IGame(*node->game);
    int status = game->GetStatus();
    while (status == game::Undecided)
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
        if (status == game::Draw)
            continue;
        if (nodes[i - 1]->game->GetPlayerToMove() == status)
            nodes[i]->value += 1;
        else
            nodes[i]->value -= 1;
    }
}

template <typename IGame>
std::vector<std::pair<game::IMove, std::pair<double, long long>>>
MCTS(const IGame &game, long long iterations)
{
    MCTSNode<IGame> root(game);
    for (long long i = 0; i < iterations; ++i)
        MCTS_(&root);

    std::vector<std::pair<game::IMove, std::pair<double, long long>>> results;
    for (auto it : root.children)
        results.emplace_back(it.first, std::pair<double, long long>{it.second->value, it.second->total});
    return results;
}

template <typename IGame>
void
MCTS01_(MCTSNode<IGame> *root)
{
    MCTSNode<IGame> *node = root;
    std::vector<MCTSNode<IGame> *> nodes = {root};
    while (true)
    {
        if (node->game->GetStatus() != game::Undecided)
            break;
        int s = node->game->GetMoveCount();
        if (static_cast<int>(node->children.size()) < s)
            break;

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

    if (node->game->GetStatus() == game::Undecided)
    {
        auto m = node->game->GetRandomMove();
        auto it = node->children.find(m);
        if (it == node->children.end())
        {
            MCTSNode<IGame> *new_node = new MCTSNode<IGame>(*node->game);
            new_node->game->ApplyMove(m);
            node->children[m] = new_node;
            node = new_node;
        }
        else
            node = it->second;
        nodes.push_back(node);
    }

    IGame *game = new IGame(*node->game);
    int status = game->GetStatus();
    while (status == game::Undecided)
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
        if (status == game::Draw)
            nodes[i]->value += 0.5;
        else if (nodes[i - 1]->game->GetPlayerToMove() == status)
            nodes[i]->value += 1.0;
    }
}

template <typename IGame>
std::vector<std::pair<game::IMove, std::pair<double, long long>>>
MCTS01(const IGame &game, long long iterations)
{
    MCTSNode<IGame> root(game);
    for (long long i = 0; i < iterations; ++i)
        MCTS01_(&root);

    std::vector<std::pair<game::IMove, std::pair<double, long long>>> results;
    for (auto it : root.children)
        results.emplace_back(it.first, {it.second->value, it.second->total});
    return results;
}

template <typename IGame>
static void
CountUnique_(const MCTSNode<IGame> *root, std::unordered_set<const IGame *> &set)
{
    set.insert(root->game);
    for (auto it : root->children)
        CountUnique_(it.second, set);
}

template <typename IGame>
long long
CountUnique(const MCTSNode<IGame> *root)
{
    std::unordered_set<const IGame *> set;
    CountUnique_<IGame>(root, set);
    return set.size();
}
