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

template <typename IGame>
void
BoardTest()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    IGame game;
    game.Print();
    std::vector<game::IMove> moves;
    while (game.GetStatus() == game::Undecided)
    {
        auto moves = game.GetPossibleMoves();
        game.ApplyMove(moves[std::uniform_int_distribution<>(0, moves.size() - 1)(gen)]);
        game.Print();
    }
}

template <typename IGame, typename MCTS>
void
MCTSTest(MCTS &&mcts, long long num_iterations)
{
    std::unordered_map<game::IMove, std::pair<double, int>> um;
    std::vector<std::pair<game::IMove, std::pair<double, int>>> best;
    int iter_num = 1;
    for (int iter = 0; iter < iter_num; ++iter)
    {
        std::cout << "Iteration: " << iter << '\n';

        auto t1 = std::chrono::high_resolution_clock::now();
        auto results = mcts({}, num_iterations);
        auto t2 = std::chrono::high_resolution_clock::now();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
        long long total = 0;
        for (const auto &result : results)
            total += result.total;
        std::cout << "MCTS done: " << total + 1 << " nodes in " << millis << " ms: " << double(total) / millis * 1000 << " nps\n";

        for (const auto &result : results)
        {
            um[result.move].first += result.value / result.total;
            um[result.move].second += result.total;
        }
    }
    for (auto it : um)
        best.push_back({it.first, {it.second.first, it.second.second}});
    std::sort(best.begin(), best.end());
    for (unsigned i = 0; i < best.size(); ++i)
        std::cout << best[i].first << ": " << best[i].second.first / iter_num << ' ' << static_cast<double>(best[i].second.second) / iter_num << '\n';
}

template <typename IGame, typename RandomPlayout=mcts::RandomPlayout<IGame>>
void
ManagerTest()
{
    IGame game;
    manager::Manager<IGame> manager(&game);

    std::vector<game::IBot<IGame> *> bots;
    bots.emplace_back(new generic_bots::FixedMctsBot<IGame, std::vector<mcts::MCTSResults> (*)(const IGame &, long long)>(&game, mcts::MCTS_parallel<IGame, RandomPlayout>, 1000));
    bots.emplace_back(new generic_bots::MinimaxBot<IGame, minimax::ABeta<IGame>>(&game, 9));

    std::vector<long long> bot_ids;
    for (auto bot : bots)
    {
        bot_ids.emplace_back(manager.AddBot(bot));
        delete bot;
    }

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
}

void
CountTest()
{
    for (int i = 0; i < 10; ++i)
    {
        std::cout << "Working on depth: " << i + 1 << '\n';

        auto t1 = std::chrono::high_resolution_clock::now();
        auto r = game::Count<uttt::IBoard>(i);
        auto t2 = std::chrono::high_resolution_clock::now();
        long long millis = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
        std::cout << "Count: " << r << '\n';
        std::cout << "Duration: " << millis / 1000 << " seconds " << millis % 1000 << " milliseconds\n";

        t1 = std::chrono::high_resolution_clock::now();
        auto count = game::CountBFS<uttt::IBoard>(i);
        t2 = std::chrono::high_resolution_clock::now();
        std::cout << "DFS count: " << count << '\n';
        millis = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
        std::cout << "Duration: " << millis / 1000 << " seconds " << millis % 1000 << " milliseconds\n";
    }
}

template <typename IGame>
void
CountUniqueBoardPositions()
{
    int depth = 20;
    long long total = 1;
    std::unordered_set<IGame> set, next_set;
    set.insert({{}});
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
}

template <typename IBot, typename IGame>
void
BotTest()
{
    IGame game;
    for (int i = 1; i < 13; ++i)
    {
        IBot bot(&game, i);
        auto t1 = std::chrono::high_resolution_clock::now();
        auto m = bot.MakeMove();
        auto t2 = std::chrono::high_resolution_clock::now();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
        std::cout << "Depth: " << i << " in " << millis << " milliseconds\n";
        std::cout << "Move: " << m << '\n';
    }
}

void
UtttHashTest()
{
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
}

int main()
{
    BoardTest<ttt::Board>();
    BoardTest<uttt::IBoard>();

    UtttHashTest();

    MCTSTest<ttt::Board>(mcts::MCTS<ttt::Board>, 100000);
    MCTSTest<ttt::Board>(mcts::MCTS_parallel<ttt::Board>, 100000);
    MCTSTest<ttt::Board>(mcts::MCTS_cache<ttt::Board>, 100000);
    MCTSTest<uttt::IBoard>(mcts::MCTS<uttt::IBoard, uttt::RandomPlayout>, 100000);
    MCTSTest<uttt::IBoard>(mcts::MCTS_parallel<uttt::IBoard, uttt::RandomPlayout>, 100000);
    MCTSTest<uttt::IBoard>(mcts::MCTS_cache<uttt::IBoard, uttt::RandomPlayout>, 100000);

    ManagerTest<ttt::Board>();
    ManagerTest<uttt::IBoard, uttt::RandomPlayout>();

    CountTest();

    BotTest<generic_bots::MinimaxBot<ttt::Board, minimax::ABeta<ttt::Board>>, ttt::Board>();
    BotTest<generic_bots::MinimaxBot<uttt::IBoard, minimax::ABeta<uttt::IBoard, uttt::Eval1>>, uttt::IBoard>();

    CountUniqueBoardPositions<ttt::Board>();
    CountUniqueBoardPositions<uttt::IBoard>();
}