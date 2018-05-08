#include <iostream>
#include <algorithm>
#include <chrono>

#include <uttt.h>
#include <mcts.h>

int main()
{
#if 0
    uttt::Board board;
    board.print();
    std::vector<char> moves;
    char player = 1;
    while (board.getStatus() == uttt::BoardStatus::Undecided)
    {
        board.getMoves(moves);
        board.applyMove(moves[0], player);
        player = 3 - player;
        board.print();
    }
#endif

    MCTSNode node;
    MCTSNode node2;

    node.game = new uttt::IBoard;
    node2.game = new uttt::IBoard;

    auto t1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000; ++i)
        MCTS2(&node2);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << "MCTS2 done: " << node2.total + 1 << " nodes in " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << '\n';

    t1 = std::chrono::high_resolution_clock::now();
    while (node.total < node2.total)
        MCTS(&node);
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << "MCTS1 done: " << node.total + 1 << " nodes in " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << '\n';

    std::vector<std::pair<IMove, MCTSNode *>> best1, best2;
    for (auto it : node.children)
        best1.emplace_back(it.first, it.second);
    for (auto it : node2.children)
        best2.emplace_back(it.first, it.second);
    std::sort(best1.begin(), best1.end());
    std::sort(best2.begin(), best2.end());

    for (unsigned i = 0; i < best1.size(); ++i)
        std::cout << best1[i].first << ": " << best1[i].second->value / best1[i].second->total << ' ' << best1[i].second->total << " vs " << best2[i].second->value / best2[i].second->total << ' ' << best2[i].second->total << '\n';
}
