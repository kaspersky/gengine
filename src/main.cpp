#include <iostream>
#include <algorithm>
#include <chrono>
#include <unordered_map>

#include <ttt.h>
#include <uttt.h>
#include <mcts.h>
#include <manager.h>
#include <generic_bots.h>

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
    int iter_num = 100;
    for (int iter = 0; iter < iter_num; ++iter)
    {
        std::cout << "Iteration: " << iter << '\n';

        auto game = new uttt::IBoard;
        MCTSNode<uttt::IBoard> node(game);
        delete game;

        auto t1 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 1; ++i)
            MCTS_cache(&node);
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
    auto game = new uttt::IBoard;
    manager::Manager<uttt::IBoard> manager(game);

    std::vector<game::IBot<uttt::IBoard> *> bots;
    bots.emplace_back(new generic_bots::MinimaxBot<uttt::IBoard, uttt::EvalMcts>(game, 1));
    bots.emplace_back(new generic_bots::FixedMctsBot<uttt::IBoard>(game, 1000));

    std::vector<long long> bot_ids;
    for (auto bot : bots)
    {
        bot_ids.emplace_back(manager.AddBot(bot));
        delete bot;
    }

    delete game;

    std::cout << "Initial ratings:";
    for (long long id : bot_ids)
        std::cout << ' ' << manager.GetBotRating(id);
    std::cout << '\n';

    int num_rounds = 2;
    for (int i = 0; i < num_rounds; ++i)
    {
        std::cout << "Round " << i + 1 << " / " << num_rounds << '\n';
        manager.RoundRobin();
        std::cout << "New ratings:";
        for (long long id : bot_ids)
            std::cout << ' ' << manager.GetBotRating(id);
        std::cout << '\n';
    }
#endif

#if 0
    auto game = new ttt::Board;
    manager::Manager<ttt::Board> manager(game);

    std::vector<game::IBot<ttt::Board> *> bots;
    bots.emplace_back(new generic_bots::RandomBot<ttt::Board>(game));
    bots.emplace_back(new generic_bots::FixedMctsBot<ttt::Board>(game, 1000));

    std::vector<long long> bot_ids;
    for (auto bot : bots)
    {
        bot_ids.emplace_back(manager.AddBot(bot));
        delete bot;
    }

    delete game;

    std::cout << "Initial ratings:";
    for (long long id : bot_ids)
        std::cout << ' ' << manager.GetBotRating(id);
    std::cout << '\n';

    int num_rounds = 10;
    for (int i = 0; i < num_rounds; ++i)
    {
        std::cout << "Round " << i + 1 << " / " << num_rounds << '\n';
        manager.RoundRobin();
        std::cout << "New ratings:";
        for (long long id : bot_ids)
            std::cout << ' ' << manager.GetBotRating(id);
        std::cout << '\n';
    }
#endif

#if 0
    auto game = new uttt::IBoard;
    MCTSNode<uttt::IBoard> node(game);
    delete game;

    auto t1 = std::chrono::high_resolution_clock::now();
    for (long long iteration = 0; ; ++iteration)
    {
        MCTS_parallel(&node);
        if ((iteration + 1) % 100000 == 0)
        {
            auto t2 = std::chrono::high_resolution_clock::now();
            std::cout << "Iterations " << iteration + 1 << ": " << node.total + 1 << " nodes in " << std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count() << " seconds\n";
            std::vector<std::pair<game::IMove, std::pair<double, int>>> best;
            for (auto it : node.children)
                best.push_back({it.first, {it.second->value / it.second->total, it.second->total}});
            std::sort(std::begin(best), std::end(best), [](const std::pair<game::IMove, std::pair<double, int>> &p1, const std::pair<game::IMove, std::pair<double, int>> &p2) {
                return p1.second.first > p2.second.first;
            });
            for (auto p : best)
                std::cout << "Move " << p.first << ": " << p.second.first << ' ' << p.second.second << '\n';
            std::cout << "Unique positions: " << CountUnique(&node) << '\n';
        }
    }
#endif

#if 0
    for (int i = 0; i < 8; ++i)
    {
        std::cout << "Working on depth: " << i + 1 << '\n';
        auto t1 = std::chrono::high_resolution_clock::now();
        auto r = game::Count<uttt::IBoard>(i);
        auto t2 = std::chrono::high_resolution_clock::now();
        std::cout << "Count: " << r.first << " / " << r.second << '\n';
        long long millis = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
        std::cout << "Duration: " << millis / 1000 << " seconds " << millis % 1000 << " milliseconds\n";
    }
#endif
}