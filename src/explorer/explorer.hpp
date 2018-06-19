#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

#include <game.h>

#include "explorer.h"

#define LEVELS_FILENAME "levels"
#define FILE_SIZE_LIMIT 512 * 1024 * 1024

namespace explorer {

template <typename IGame, typename Reader, int Size>
static void
LoadFromFile(const std::string &filename, std::vector<IGame> &data)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.good())
        return;

    long long total_bytes = 0;

    long long count = 0;
    char count_buf[8];

    std::cout << "Reading " << count << " states from " << filename << '\n';

    auto t1 = std::chrono::high_resolution_clock::now();
    file.read(count_buf, 8);
    for (int i = 0; i < 8; ++i)
        count = (count << 8) | uint8_t(count_buf[i]);
    total_bytes += 8;

    for (long long i = 0; i < count; ++i)
    {
        std::vector<char> bytes(Size);
        file.read(&bytes[0], Size);
        total_bytes += Size;
        data.emplace_back(Reader()(bytes));
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    std::cout << "Reading done: " << total_bytes << " bytes in " << millis / 1000 << " s " << millis % 1000 << " ms: " << double(total_bytes) / 1024 * 1000 / 1024 / millis << " MB/s\n";
}

template <typename IGame, typename Writer>
static void
DumpToFile(const std::string &filename, const std::vector<IGame> &data)
{
    long long total_bytes = 0, count = data.size();
    char count_buf[8];
    for (int i = 7; i >= 0; --i)
    {
        count_buf[i] = count & 0xff;
        count >>= 8;
    }
    
    std::cout << "Writing " << data.size() << " states to " << filename << '\n';
    std::ofstream file(filename, std::ios::binary);

    auto t1 = std::chrono::high_resolution_clock::now();
    file.write(count_buf, 8);
    total_bytes += 8;

    for (const auto &d : data)
    {
        auto bytes = Writer()(d);
        file.write(&bytes[0], bytes.size());
        total_bytes += bytes.size();
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    std::cout << "Writing done: " << total_bytes << " bytes in " << millis / 1000 << " s " << millis % 1000 << " ms: " << double(total_bytes) / 1024 * 1000 / 1024 / millis << " MB/s\n";
}

static bool
ReadLevelsFile(const std::string &directory, int &next_level, int &offset, int &next_offset)
{
    std::ifstream in(directory + '/' + LEVELS_FILENAME);
    if (!in.good())
        return false;
    in >> next_level >> offset >> next_offset;
    return true;
}

static void
WriteLevelsFile(const std::string &directory, int next_level, int offset, int next_offset)
{
    std::ofstream out(directory + '/' + LEVELS_FILENAME);
    out << next_level << ' ' << offset << ' ' << next_offset << '\n';
}

template <typename IGame, typename Reader, typename Writer, int Size>
static long long
ExploreLevel(const std::string &directory, int next_level, int offset, int next_offset)
{
    std::vector<IGame> next_data;
    long long games_count = 0;

    std::cout << "Exploring level: " << next_level << '\n';

    auto t1 = std::chrono::high_resolution_clock::now();

    while (true)
    {
        std::vector<IGame> data;
        LoadFromFile<IGame, Reader, Size>(directory + '/' + std::to_string(next_level - 1) + '_' + std::to_string(offset), data);
        if (data.empty())
            break;

        for (const auto &g : data)
        {
            if (g.GetStatus() != game::Undecided)
                continue;
            auto moves = g.GetPossibleMoves();
            IGame game;
            for (auto m : moves)
            {
                auto ng = g;
                ng.ApplyMove(m);
                next_data.emplace_back(ng);
                ++games_count;
                if (next_data.size() * Size >= FILE_SIZE_LIMIT)
                {
                    DumpToFile<IGame, Writer>(directory + '/' + std::to_string(next_level) + '_' + std::to_string(next_offset), next_data);
                    ++next_offset;
                    next_data.clear();
                }
            }
        }
        ++offset;
        WriteLevelsFile(directory, next_level, offset, next_offset);
    }

    if (!next_data.empty())
        DumpToFile<IGame, Writer>(directory + '/' + std::to_string(next_level) + '_' + std::to_string(next_offset), next_data);

    WriteLevelsFile(directory, next_level + 1, 0, 0);

    auto t2 = std::chrono::high_resolution_clock::now();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    std::cout << "Level " << next_level << " completed in " << millis / 1000 << "s " << millis % 1000 << "ms, " << double(games_count) * 1000 / millis << " nps\n";

    return games_count;
}

template <typename IGame, typename Reader, typename Writer, int Size>
void
Explore(const std::string &directory)
{
    int next_level = 0, offset = 0, next_offset = 0;
    if (!ReadLevelsFile(directory, next_level, offset, next_offset))
    {
        DumpToFile<IGame, Writer>(directory + "/0_0", {{}});
        next_level = 1, offset = 0, next_offset = 0;
        WriteLevelsFile(directory, next_level, offset, next_offset);
    }

    ExploreLevel<IGame, Reader, Writer, Size>(directory, next_level, offset, next_offset);
    for (int level = next_level + 1; ; ++level)
        ExploreLevel<IGame, Reader, Writer, Size>(directory, level, 0, 0);
}

}