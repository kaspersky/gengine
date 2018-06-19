#pragma once

#include <vector>
#include <array>

#include <game.h>

namespace uttt {

const int IBoardSize = 24;

struct RandomPlayout;
struct RandomPlayout2;
struct IBoardReader;
struct IBoardWriter;
struct Eval1;

class IBoard
{
    friend RandomPlayout;
    friend RandomPlayout2;
    friend IBoardReader;
    friend IBoardWriter;
    friend Eval1;

    int32_t macro;
    std::array<int16_t, 9> micro;
    int8_t next;
    int8_t player;

    void RotateClockWise90();

    void MirrorVertical();

public:
    IBoard();

    IBoard(int macro, const std::array<short, 9> &micro, int8_t next, char player);

    bool operator==(const IBoard &other) const;

    std::vector<game::IMove> GetPossibleMoves() const;

    void ApplyMove(const game::IMove &move);

    int GetStatus() const;

    game::IPlayer GetPlayerToMove() const;

    std::size_t Hash() const;

    void Print() const;
};

struct IBoardReader
{
    std::vector<IBoard> operator()(const std::vector<char> &bytes) const
    {
        std::vector<IBoard> boards(bytes.size() / IBoardSize);
        for (unsigned long long offset = 0; offset < bytes.size(); offset += IBoardSize)
        {
            auto &b = boards[offset / IBoardSize];
            auto it = &bytes[offset];
            b.macro = (uint8_t(it[0]) << 24) | (uint8_t(it[1]) << 16) | (uint8_t(it[2]) << 8) | uint8_t(it[3]);
            b.micro[0] = (uint8_t(it[4]) << 8) | uint8_t(it[5]);
            b.micro[1] = (uint8_t(it[6]) << 8) | uint8_t(it[7]);
            b.micro[2] = (uint8_t(it[8]) << 8) | uint8_t(it[9]);
            b.micro[3] = (uint8_t(it[10]) << 8) | uint8_t(it[11]);
            b.micro[4] = (uint8_t(it[12]) << 8) | uint8_t(it[13]);
            b.micro[5] = (uint8_t(it[14]) << 8) | uint8_t(it[15]);
            b.micro[6] = (uint8_t(it[16]) << 8) | uint8_t(it[17]);
            b.micro[7] = (uint8_t(it[18]) << 8) | uint8_t(it[19]);
            b.micro[8] = (uint8_t(it[20]) << 8) | uint8_t(it[21]);
            b.next = it[22];
            b.player = it[23];
        }
        return boards;
    }
};

struct IBoardWriter
{
    std::vector<char> operator()(const std::vector<IBoard> &boards) const
    {
        std::vector<char> bytes(IBoardSize * boards.size());
        long long offset = 0;
        for (const auto &board : boards)
        {
            auto it = &bytes[offset];
            it[0] = uint32_t(board.macro) >> 24;
            it[1] = (uint32_t(board.macro) >> 16) & 0xff;
            it[2] = (uint32_t(board.macro) >> 8) & 0xff;
            it[3] = uint32_t(board.macro) & 0xff;
            it[4] = uint16_t(board.micro[0]) >> 8;
            it[5] = board.micro[0] & 0xff;
            it[6] = uint16_t(board.micro[1]) >> 8;
            it[7] = board.micro[1] & 0xff;
            it[8] = uint16_t(board.micro[2]) >> 8;
            it[9] = board.micro[2] & 0xff;
            it[10] = uint16_t(board.micro[3]) >> 8;
            it[11] = board.micro[3] & 0xff;
            it[12] = uint16_t(board.micro[4]) >> 8;
            it[13] = board.micro[4] & 0xff;
            it[14] = uint16_t(board.micro[5]) >> 8;
            it[15] = board.micro[5] & 0xff;
            it[16] = uint16_t(board.micro[6]) >> 8;
            it[17] = board.micro[6] & 0xff;
            it[18] = uint16_t(board.micro[7]) >> 8;
            it[19] = board.micro[7] & 0xff;
            it[20] = uint16_t(board.micro[8]) >> 8;
            it[21] = board.micro[8] & 0xff;
            it[22] = board.next;
            it[23] = board.player;
            offset += IBoardSize;
        }
        return bytes;
    }
};

class UtttBot: public game::IBot<IBoard>
{
    IBoard board;
    long long mcts_iterations;

    UtttBot(const IBoard &board, long long mcts_iterations);

public:
    UtttBot(long long mcts_iterations);
    UtttBot(const UtttBot &other);

    game::IMove MakeMove();

    void SendMove(const game::IMove &move);

    game::IBot<IBoard> *Clone() const;
};

struct Eval1
{
    double operator()(const IBoard &board) const;
};

struct EvalMcts
{
    double operator()(const IBoard &board) const;
};

struct RandomPlayout
{
    int operator()(const uttt::IBoard &game) const;
};

struct RandomPlayout2
{
    int operator()(const uttt::IBoard &game) const;
};

}

namespace std {
    template <>
    struct hash<uttt::IBoard>
    {
        std::size_t operator()(const uttt::IBoard &board) const
        {
            return board.Hash();
        }
    };
}