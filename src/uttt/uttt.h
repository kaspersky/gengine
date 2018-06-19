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
    template <typename InputIterator>
    IBoard operator()(InputIterator iterator) const
    {
        int32_t macro = 0;
        std::array<int16_t, 9> micro = {};
        int8_t next, player = 0;

        for (int i = 0; i < 4; ++i)
            macro = (macro << 8) | uint8_t(*(iterator++));
        for (int i = 0; i < 9; ++i)
        {
            micro[i] = uint8_t(*(iterator++)) << 8;
            micro[i] |= uint8_t(*(iterator++));
        }
        next = *(iterator++);
        player = *(iterator++);

        return IBoard(macro, micro, next, player);
    }
};

struct IBoardWriter
{
    void operator()(const IBoard &board, std::vector<char> &bytes) const
    {
        bytes.resize(bytes.size() + IBoardSize);
        auto it = &bytes[bytes.size() - IBoardSize];
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