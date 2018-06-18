#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

#include "uttt.h"

#define LEVELS_FILENAME "levels"
#define FILE_SIZE_LIMIT 100000000
#define UTTT_IBOARD_SIZE 24

static void
LoadFile(const std::string &filename, std::vector<uttt::IBoard> &data)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.good())
        return;

    long long count = 0;
    char count_buf[8];
    file.read(count_buf, 8);
    for (int i = 0; i < 8; ++i)
        count = (count << 8) | uint8_t(count_buf[i]);
    std::cout << "Reading " << count << " states...\n";
    for (long long i = 0; i < count; ++i)
    {
        std::vector<char> bytes(UTTT_IBOARD_SIZE);
        file.read(&bytes[0], UTTT_IBOARD_SIZE);
        data.emplace_back(uttt::IBoard::FromBytes(bytes));
    }
    std::cout << "Reading done\n";
}

static void
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
        file.write(&uttt::IBoard::ToBytes(d)[0], UTTT_IBOARD_SIZE);
        total_bytes += UTTT_IBOARD_SIZE;
    }

    std::cout << "Writing done: " << total_bytes << " bytes\n";
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

template <typename IGame>
static void
ExploreLevel(const std::string &directory, int next_level, int offset, int next_offset)
{
    std::vector<IGame> next_data;
    while (true)
    {
        std::vector<IGame> data;
        LoadFile(directory + '/' + std::to_string(next_level - 1) + "_" + std::to_string(offset), data);
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
                if (next_data.size() * UTTT_IBOARD_SIZE >= FILE_SIZE_LIMIT)
                {
                    DumpToFile(directory + '/' + std::to_string(next_level) + '_' + std::to_string(next_offset), next_data);
                    ++next_offset;
                    next_data.clear();
                }
            }
        }
        ++offset;
        WriteLevelsFile(directory, next_level, offset, next_offset);
    }

    if (!next_data.empty())
        DumpToFile(directory + '/' + std::to_string(next_level) + '_' + std::to_string(next_offset), next_data);

    WriteLevelsFile(directory, next_level + 1, 0, 0);
}

template <typename IGame>
static void
Explore(const std::string &directory)
{
    int next_level = 0, offset = 0, next_offset = 0;
    if (!ReadLevelsFile(directory, next_level, offset, next_offset))
    {
        DumpToFile(directory + "/0_0", {{}});
        next_level = 1, offset = 0, next_offset = 0;
        WriteLevelsFile(directory, next_level, offset, next_offset);
    }

    ExploreLevel<IGame>(directory, next_level, offset, next_offset);
    for (int level = next_level + 1; ; ++level)
        ExploreLevel<IGame>(directory, level, 0, 0);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <dir_path>\n";
        return 0;
    }

    Explore<uttt::IBoard>(argv[1]);
}