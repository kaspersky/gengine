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

#if 0
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

#if 1
    manager::Manager manager;

    game::IGame *game = new uttt::IBoard;
    long long game_id = manager.AddGame(game);
    delete game;

    auto bot1 = new uttt::UtttBot(10);
    auto bot2 = new uttt::UtttBot(100);
    auto bot3 = new uttt::UtttBot(1000);
    long long bot1_id = manager.AddBot(bot1, game_id);
    long long bot2_id = manager.AddBot(bot2, game_id);
    long long bot3_id = manager.AddBot(bot3, game_id);
    delete bot1;
    delete bot2;
    delete bot3;

    std::cout << "Initial ratings: " << manager.GetBotRating(bot1_id) << " vs " << manager.GetBotRating(bot2_id) << '\n';
    int num_rounds = 1;
    for (int i = 0; i < num_rounds; ++i)
    {
        std::cout << "Round " << i + 1 << " / " << num_rounds << '\n';
        manager.RoundRobin(game_id);
        std::cout << "New ratings: " << manager.GetBotRating(bot1_id) << " vs " << manager.GetBotRating(bot2_id) << " vs " << manager.GetBotRating(bot3_id) << '\n';
    }
#endif
}