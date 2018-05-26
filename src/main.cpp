#include <iostream>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <unordered_set>

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
    while (board.GetStatus() == game::Undecided)
    {
        board.ApplyMove(board.GetRandomMove());
        board.Print();
    }
#endif

#if 0
    uttt::IBoard board;
    std::unordered_set<uttt::IBoard> set1, set2;
    set1.insert(board);
    for (int i = 0; i < 2; ++i)
    {

        set2.clear();
        for (auto b : set1)
        {
            auto moves = b.GetPossibleMoves();
            for (auto m : moves)
            {
                auto b2 = b;
                b2.ApplyMove(m);
                set2.insert(b2);
            }
        }
        set1 = set2;
        std::unordered_map<std::size_t, std::vector<uttt::IBoard>> hashes;
        for (auto b : set1)
            hashes[b.Hash()].push_back(b);
        std::cout << "After iteration " << i + 1 << " set size = " << set1.size() << '\n';
        std::cout << "hashes size = " << hashes.size() << '\n';

        for (auto it : hashes)
        {
            std::cout << "Hash family: " << it.first << '\n';
            for (auto b : it.second)
                b.Print();
        }

    }
#endif

#if 1
    std::unordered_map<game::IMove, std::pair<double, int>> um;
    std::vector<std::pair<game::IMove, std::pair<double, int>>> best;
    int iter_num = 1;
    for (int iter = 0; iter < iter_num; ++iter)
    {
        std::cout << "Iteration: " << iter << '\n';

        auto t1 = std::chrono::high_resolution_clock::now();
        auto results = MCTS_cache<uttt::IBoard>(uttt::IBoard(), 50000);
        auto t2 = std::chrono::high_resolution_clock::now();
        long long total = 0;
        for (auto it : results)
            total += it.second.second;
        std::cout << "MCTS done: " << total + 1 << " nodes in " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << '\n';

        for (auto it : results)
        {
            um[it.first].first += it.second.first / it.second.second;
            um[it.first].second += it.second.second;
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
    bots.emplace_back(new generic_bots::FixedMctsBot<uttt::IBoard, MCTS_parallel>(game, 1000));
    bots.emplace_back(new generic_bots::FixedMctsBot<uttt::IBoard, MCTS_cache>(game, 1000));

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

    int num_rounds = 200;
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
    for (int i = 0; i < 10; ++i)
    {
        std::cout << "Working on depth: " << i + 1 << '\n';

        auto t1 = std::chrono::high_resolution_clock::now();
        auto r = game::Count<uttt::IBoard>(i);
        auto t2 = std::chrono::high_resolution_clock::now();
        long long millis = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
        std::cout << "Count: " << r << '\n';
        std::cout << "Duration: " << millis / 1000 << " seconds " << millis % 1000 << " milliseconds\n";
    }
#endif

#if 0
    for (int i = 0; i < 10; ++i)
    {
        std::cout << "Working on depth: " << i + 1 << '\n';

        auto t1 = std::chrono::high_resolution_clock::now();
        auto count = game::CountBFS<uttt::IBoard>(i);
        auto t2 = std::chrono::high_resolution_clock::now();
        std::cout << "DFS count: " << count << '\n';
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
        std::cout << "Duration: " << millis / 1000 << " seconds " << millis % 1000 << " milliseconds\n";
    }
#endif

#if 0
    int depth = 20;
    long long total = 1;
    std::unordered_set<uttt::IBoard> set, next_set;
    set.insert(uttt::IBoard());
    for (int i = 0; i < depth; ++i)
    {
        std::cout << "Working on depth: " << i + 1 << '\n';
        auto t1 = std::chrono::high_resolution_clock::now();
        next_set.clear();
        for (auto g : set)
        {
            if (g.GetStatus() != game::Undecided)
                continue;
            auto moves = g.GetPossibleMoves();
            for (auto m : moves)
            {
                auto ng = g;
                ng.ApplyMove(m);
                next_set.insert(ng);
            }
        }
        total += next_set.size();
        set = next_set;
        auto t2 = std::chrono::high_resolution_clock::now();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
        std::cout << "Total count: " << total << '\n';
        std::cout << "Duration: " << millis / 1000 << " seconds " << millis % 1000 << " milliseconds\n";
    }
#endif
}