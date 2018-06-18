#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

#include "uttt.h"

void
LoadFile(const std::string &filename, std::vector<uttt::IBoard> &data)
{
    std::ifstream file(filename, std::ios::binary);
    long long count = 0;
    char count_buf[8];
    file.read(count_buf, 8);
    for (int i = 0; i < 8; ++i)
        count = (count << 8) | count_buf[i];
    std::cout << "Reading " << count << " states...\n";
    for (long long i = 0; i < count; ++i)
    {
        int size = 24;
        std::vector<char> bytes(size);
        file.read(&bytes[0], size);
        data.emplace_back(uttt::IBoard::FromBytes(bytes));
    }
    std::cout << "Reading done\n";
}

void
DumpToFile(const std::string &filename, const std::vector<uttt::IBoard> &data)
{
    long long total_bytes = 0;
    std::cout << "Writing " << data.size() << " states...\n";
    std::ofstream file(filename, std::ios::binary);
    long long count = data.size();
    char count_buf[8];
    for (int i = 7; i >= 0; --i)
    {
        count_buf[i] = count & 0xff;
        count >>= 8;
    }
    file.write(count_buf, 8);
    total_bytes += 8;

    for (const auto &d : data)
    {
        file.write(&uttt::IBoard::ToBytes(d)[0], 24);
        total_bytes += 24;
    }

    std::cout << "Writing done: " << total_bytes << " bytes\n";
}

template <typename IGame>
void
static CountBFS(int depth, const std::string &filename)
{
    std::cout << "Game size: " << sizeof(IGame) << " bytes\n";
    long long total = 1;
    std::vector<IGame> levels[2] = {{{}}};
    int current_level = 0;
    DumpToFile(filename + "0", levels[current_level]);
    for (int i = 1; i <= depth; ++i)
    {
        std::cout << "Working on depth: " << i << '\n';
        levels[1 - current_level].clear();
        auto t1 = std::chrono::high_resolution_clock::now();
        for (const auto &g : levels[current_level])
        {
            if (g.GetStatus() != game::Undecided)
                continue;
            auto moves = g.GetPossibleMoves();
            for (auto m : moves)
            {
                auto ng = g;
                ng.ApplyMove(m);
                levels[1 - current_level].emplace_back(ng);
            }
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
        total += levels[1 - current_level].size();
        std::cout << "Current depth count: " << levels[1 - current_level].size() << ", size: " << levels[1 - current_level].size() * sizeof(IGame) / (1 << 20) << "MB\n";
        std::cout << "Total count: " << total << ", size: " << total * sizeof(IGame) / (1 << 20) << " MB\n";
        std::cout << "Duration: " << millis / 1000 << " seconds " << millis % 1000 << " milliseconds, " << double(levels[1 - current_level].size()) / millis * 1000 << " nps\n";
        DumpToFile(filename + std::to_string(i), levels[1 - current_level]);
        current_level = 1 - current_level;
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <dir_path>\n";
        return 0;
    }
    CountBFS<uttt::IBoard>(10, argv[1]);
}