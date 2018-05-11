#include <iostream>
#include <algorithm>
#include <chrono>
#include <unordered_map>

#include <uttt.h>
#include <mcts.h>
#include <manager.h>

int main()
{
#if 0
    uttt::IBoard board;
    board.Print();
    std::vector<game::IMove> moves;
    while (board.GetStatus() == game::IGame::Undecided)
    {
        board.ApplyMove(board.GetRandomMove());
        board.Print();
    }
#endif

#if 1
    std::unordered_map<game::IMove, std::pair<double, int>> um;
    std::vector<std::pair<game::IMove, std::pair<double, int>>> best;
    int iter_num = 50;
    for (int iter = 0; iter < iter_num; ++iter)
    {
        std::cout << "Iteration: " << iter << '\n';
        MCTSNode node;

        node.game = new uttt::IBoard;

        auto t1 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 100000; ++i)
            MCTS(&node);
        auto t2 = std::chrono::high_resolution_clock::now();
        std::cout << "MCTS done: " << node.total + 1 << " nodes in " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << '\n';

        for (auto it : node.children)
        {
            um[it.first].first += it.second->value / it.second->total;
            um[it.first].second += it.second->total;
        }
    }
    for (auto it : um)
        best.push_back({it.first, {it.second.first, it.second.second}});
    std::sort(best.begin(), best.end());
    for (unsigned i = 0; i < best.size(); ++i)
        std::cout << best[i].first << ": " << best[i].second.first / iter_num << ' ' << static_cast<double>(best[i].second.second) / iter_num << '\n';
#endif

#if 0
    manager::Manager manager;
    game::IBot *bot1 = new uttt::UtttBot, *bot2 = new uttt::UtttBot;
    game::IGame *game = new uttt::IBoard;

    auto status = manager.Match(bot1, bot2, game);
    std::cout << "Status: " << status << '\n';

    delete bot1;
    delete bot2;
    delete game;
#endif
}
