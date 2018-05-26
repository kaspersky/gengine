#include <algorithm>
#include <iostream>
#include <vector>
#include <array>
#include <limits>
#include <random>

#include <uttt.h>
#include <mcts.h>

typedef std::array<std::array<int, 3>, 3> TBoard;

static short g_factors[9] = {1, 3, 9, 27, 81, 243, 729, 2187, 6561};
static int g_factors4[9] = {1, 4, 16, 64, 256, 1024, 4096, 16384, 65536};
static int8_t g_next_rotated_clockwise_90[9] = {2, 5, 8, 1, 4, 7, 0, 3, 6};
static int8_t g_next_mirrored_vertical[9] = {2, 1, 0, 5, 4, 3, 8, 7, 6};
static char g_board_status[19683];
static char g_macro_board_status[262144];
static std::vector<char> g_moves[19683];
static int g_rotated_clockwise_90[19683];
static int g_mirrored_vertical[19683];
static int g_macro_rotated_clockwise_90[262144];
static int g_macro_mirrored_vertical[262144];
static std::size_t g_hash[9][19683];
static std::size_t g_hash4[262144];

static TBoard
makeBoard(short k)
{
    return {k / g_factors[0] % 3, k / g_factors[1] % 3, k / g_factors[2] % 3,
            k / g_factors[3] % 3, k / g_factors[4] % 3, k / g_factors[5] % 3,
            k / g_factors[6] % 3, k / g_factors[7] % 3, k / g_factors[8] % 3};
}

static TBoard
makeBoard4(int k)
{
    return {k / g_factors4[0] % 4, k / g_factors4[1] % 4, k / g_factors4[2] % 4,
            k / g_factors4[3] % 4, k / g_factors4[4] % 4, k / g_factors4[5] % 4,
            k / g_factors4[6] % 4, k / g_factors4[7] % 4, k / g_factors4[8] % 4};
}

static int
TBoardToInt(const TBoard &board)
{
    return board[0][0] * g_factors[0] + board[0][1] * g_factors[1] + board[0][2] * g_factors[2] +
           board[1][0] * g_factors[3] + board[1][1] * g_factors[4] + board[1][2] * g_factors[5] +
           board[2][0] * g_factors[6] + board[2][1] * g_factors[7] + board[2][2] * g_factors[8];
}

static int
TBoardToInt4(const TBoard &board)
{
    return board[0][0] * g_factors4[0] + board[0][1] * g_factors4[1] + board[0][2] * g_factors4[2] +
           board[1][0] * g_factors4[3] + board[1][1] * g_factors4[4] + board[1][2] * g_factors4[5] +
           board[2][0] * g_factors4[6] + board[2][1] * g_factors4[7] + board[2][2] * g_factors4[8];
}

static void
RotateClockWise90(TBoard &board)
{
    std::swap(board[0][1], board[1][0]);
    std::swap(board[1][0], board[2][1]);
    std::swap(board[2][1], board[1][2]);
    std::swap(board[0][0], board[2][0]);
    std::swap(board[2][0], board[2][2]);
    std::swap(board[2][2], board[0][2]);
}

static void
MirrorVertical(TBoard &board)
{
    std::swap(board[0][0], board[0][2]);
    std::swap(board[1][0], board[1][2]);
    std::swap(board[2][0], board[2][2]);
}

static uint8_t
getBoardStatus(const TBoard &board)
{
    for (int i = 0; i < 3; i++)
    {
        if (board[i][1] == board[i][0] && board[i][1] == board[i][2] && board[i][1] != 0)
            return (board[i][1] == 1 ? 1 : 2);
        if (board[1][i] == board[0][i] && board[1][i] == board[2][i] && board[1][i] != 0)
            return (board[1][i] == 1 ? 1 : 2);
    }
    if ((board[1][1] == board[0][0] && board[1][1] == board[2][2] && board[1][1] != 0) ||
        (board[1][1] == board[2][0] && board[1][1] == board[0][2] && board[1][1] != 0))
        return (board[1][1] == 1 ? 1 : 2);
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (board[i][j] == 0)
                return 0;
    return 3;
}

static char
getMacroBoardStatus(const TBoard &board)
{
    for (int i = 0; i < 3; i++)
    {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != 0 && board[i][0] != 3)
            return (board[i][0] == 1 ? 1 : 2);
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != 0 && board[0][i] != 3)
            return (board[0][i] == 1 ? 1 : 2);
    }
    if ((board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != 0 && board[0][0] != 3) ||
        (board[2][0] == board[1][1] && board[1][1] == board[0][2] && board[2][0] != 0 && board[2][0] != 3))
        return (board[1][1] == 1 ? 1 : 2);
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (board[i][j] == 0)
                return 0;
    return 3;
}

struct UtttInit
{
    UtttInit()
    {
        for (short i = 0; i < 19683; i++)
        {
            g_board_status[i] = getBoardStatus(makeBoard(i));
            if (g_board_status[i] == 0)
                for (int j = 0; j < 9; j++)
                    if ((i / g_factors[j]) % 3 == 0)
                        g_moves[i].push_back(j);
            auto board = makeBoard(i);
            g_hash[0][i] = board[0][0] ^ ((board[0][1] ^ board[1][0]) << 2) ^ ((board[0][2] ^ board[2][0]) << 4) ^ (board[1][1] << 6) ^ ((board[1][2] ^ board[2][1]) << 8) ^ (board[2][2] << 10);
            g_hash[2][i] = board[0][2] ^ ((board[0][1] ^ board[1][2]) << 2) ^ ((board[0][0] ^ board[2][2]) << 4) ^ (board[1][1] << 6) ^ ((board[1][0] ^ board[2][1]) << 8) ^ (board[2][0] << 10);
            g_hash[6][i] = board[2][0] ^ ((board[1][0] ^ board[2][1]) << 2) ^ ((board[0][0] ^ board[2][2]) << 4) ^ (board[1][1] << 6) ^ ((board[0][1] ^ board[1][2]) << 8) ^ (board[0][2] << 10);
            g_hash[8][i] = board[2][2] ^ ((board[1][2] ^ board[2][1]) << 2) ^ ((board[0][2] ^ board[2][0]) << 4) ^ (board[1][1] << 6) ^ ((board[0][1] ^ board[1][0]) << 8) ^ (board[0][0] << 10);
            g_hash[1][i] = (board[0][1] << 12) ^ (board[1][1] << 14) ^ (board[2][1] << 16) ^ ((board[0][0] ^ board[0][2]) << 18) ^ ((board[1][0] ^ board[1][2]) << 20) << ((board[2][0] ^ board[2][2]) << 22);
            g_hash[3][i] = (board[1][0] << 12) ^ (board[1][1] << 14) ^ (board[1][2] << 16) ^ ((board[0][0] ^ board[2][0]) << 18) ^ ((board[0][1] ^ board[2][1]) << 20) << ((board[0][2] ^ board[2][2]) << 22);
            g_hash[5][i] = (board[1][2] << 12) ^ (board[1][1] << 14) ^ (board[1][0] << 16) ^ ((board[0][2] ^ board[2][2]) << 18) ^ ((board[0][1] ^ board[2][1]) << 20) << ((board[0][0] ^ board[2][0]) << 22);
            g_hash[7][i] = (board[2][1] << 12) ^ (board[1][1] << 14) ^ (board[0][1] << 16) ^ ((board[2][0] ^ board[2][2]) << 18) ^ ((board[1][0] ^ board[1][2]) << 20) << ((board[0][0] ^ board[0][2]) << 22);
            g_hash[4][i] = (board[1][1] << 24) ^ ((board[0][0] ^ board[0][2] ^ board[2][0] ^ board[2][2]) << 26) ^ ((board[0][1] ^ board[1][0] ^ board[1][2] ^ board[2][1]) << 28);
            RotateClockWise90(board);
            g_rotated_clockwise_90[i] = TBoardToInt(board);
            board = makeBoard(i);
            MirrorVertical(board);
            g_mirrored_vertical[i] = TBoardToInt(board);
        }

        for (int i = 0; i < 262144; i++)
        {
            g_macro_board_status[i] = getMacroBoardStatus(makeBoard4(i));
            auto board = makeBoard4(i);
            g_hash4[i] = (board[1][1] << 4) ^ ((board[0][0] ^ board[0][2] ^ board[2][0] ^ board[2][2]) << 2) ^ board[0][1] ^ board[1][0] ^ board[1][2] ^ board[2][1];
            RotateClockWise90(board);
            g_macro_rotated_clockwise_90[i] = TBoardToInt4(board);
            board = makeBoard4(i);
            MirrorVertical(board);
            g_macro_mirrored_vertical[i] = TBoardToInt4(board);
        }
    }
};

static UtttInit g_UtttInit;

namespace uttt {

IBoard::IBoard(): macro(0), micro({}), next(-1), player(1)
{
}

IBoard::IBoard(int macro, const std::array<short, 9> &micro, int8_t next, char player): macro(macro), micro(micro), next(next), player(player)
{
}

bool
IBoard::operator==(const IBoard &other) const
{
    if (macro == other.macro && micro == other.micro && next == other.next)
        return true;
    IBoard rotated(*this);
    rotated.RotateClockWise90();
    if (rotated.macro == other.macro && rotated.micro == other.micro && rotated.next == other.next)
        return true;
    rotated.RotateClockWise90();
    if (rotated.macro == other.macro && rotated.micro == other.micro && rotated.next == other.next)
        return true;
    rotated.RotateClockWise90();
    if (rotated.macro == other.macro && rotated.micro == other.micro && rotated.next == other.next)
        return true;

    IBoard vertical(*this);
    vertical.MirrorVertical();
    if (vertical.macro == other.macro && vertical.micro == other.micro && vertical.next == other.next)
        return true;
    vertical.RotateClockWise90();
    if (vertical.macro == other.macro && vertical.micro == other.micro && vertical.next == other.next)
        return true;
    vertical.RotateClockWise90();
    if (vertical.macro == other.macro && vertical.micro == other.micro && vertical.next == other.next)
        return true;
    vertical.RotateClockWise90();
    if (vertical.macro == other.macro && vertical.micro == other.micro && vertical.next == other.next)
        return true;
    return false;
}

void
IBoard::RotateClockWise90()
{
    macro = g_macro_rotated_clockwise_90[macro];
    for (auto &m : micro)
        m = g_rotated_clockwise_90[m];
    micro = {micro[6], micro[3], micro[0], micro[7], micro[4], micro[1], micro[8], micro[5], micro[2]};
    if (next != -1)
        next = g_next_rotated_clockwise_90[next];
}

void
IBoard::MirrorVertical()
{
    macro = g_macro_mirrored_vertical[macro];
    for (auto &m : micro)
        m = g_mirrored_vertical[m];
    micro = {micro[2], micro[1], micro[0], micro[5], micro[4], micro[3], micro[8], micro[7], micro[6]};
    if (next != -1)
        next = g_next_mirrored_vertical[next];
}

std::size_t
IBoard::Hash() const
{
    std::size_t result = g_hash4[macro];
    result <<= 6;
    result ^= g_hash[4][micro[4]];
    result ^= g_hash[0][micro[0]] ^ g_hash[2][micro[2]] ^ g_hash[6][micro[6]] ^ g_hash[8][micro[8]];
    result ^= g_hash[1][micro[1]] ^ g_hash[3][micro[3]] ^ g_hash[5][micro[5]] ^ g_hash[7][micro[7]];
    return result;
}

void
IBoard::Print() const
{
    std::cout << "Player to move: " << static_cast<int>(player) << '\n';
    std::cout << static_cast<int>(next) << '\n';
    auto board = makeBoard4(macro);
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
            std::cout << board[i][j];
        std::cout << '\n';
    }
    int data[9][9];
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            data[i / 3 * 3 + j / 3][i % 3 * 3 + j % 3] = micro[i] / g_factors[j] % 3;
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
            std::cout << static_cast<int>(data[i][j]) << ' ';
        std::cout << '\n';
    }
    std::cout << "Status: " << static_cast<int>(GetStatus()) << '\n';
    std::cout << "Hash: " << Hash() << "\n\n";
}

std::vector<game::IMove>
IBoard::GetPossibleMoves() const
{
    std::vector<game::IMove> moves;
    if (next >= 0)
        for (auto m : g_moves[micro[next]])
            moves.emplace_back(next * 9 + m);
    else
        for (int i = 0; i < 9; i++)
            for (auto m : g_moves[micro[i]])
                moves.emplace_back(i * 9 + m);
    return moves;
}

int
IBoard::GetMoveCount() const
{
    return GetPossibleMoves().size();
}

game::IMove
IBoard::GetRandomMove() const
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    auto moves = GetPossibleMoves();
    return moves[std::uniform_int_distribution<>(0, moves.size() - 1)(gen)];
}

void
IBoard::ApplyMove(const game::IMove &move)
{
    micro[move / 9] += player * g_factors[move % 9];
    macro += g_board_status[micro[move / 9]] * g_factors4[move / 9];
    if (g_board_status[micro[move % 9]] == 0)
        next = move % 9;
    else
        next = -1;
    player = 3 - player;
}

int
IBoard::GetStatus() const
{
    auto status = g_macro_board_status[macro];
    if (status == 0)
        return game::Undecided;
    if (status == 3)
        return game::Draw;
    return status;
}

game::IPlayer
IBoard::GetPlayerToMove() const
{
    return player;
}

UtttBot::UtttBot(long long mcts_iterations): game::IBot<IBoard>(nullptr), mcts_iterations(mcts_iterations)
{
}

UtttBot::UtttBot(const IBoard &board, long long mcts_iterations): game::IBot<IBoard>(nullptr), board(board), mcts_iterations(mcts_iterations)
{
}

UtttBot::UtttBot(const UtttBot &other): game::IBot<IBoard>(other), board(other.board), mcts_iterations(other.mcts_iterations)
{
}

game::IMove
UtttBot::MakeMove()
{
    auto results = MCTS(board, mcts_iterations);
    double max = -1.1;
    game::IMove move = -1;
    for (auto it : results)
    {
        auto v = it.second.first / it.second.second;
        if (v > max)
            max = v, move = it.first;
    }
    board.ApplyMove(move);
    return move;
}

void
UtttBot::SendMove(const game::IMove &move)
{
    board.ApplyMove(move);
}

game::IBot<IBoard> *
UtttBot::Clone() const
{
    return new UtttBot(*this);
}

double
Eval::operator()(const IBoard *board) const
{
    int status = board->GetStatus();
    if (status == game::Draw)
        return 0.0;
    if (status != game::Undecided)
    {
        if (board->GetPlayerToMove() == status)
            return std::numeric_limits<double>::max();
        return std::numeric_limits<double>::min();
    }
    return 0.0;
}

double
EvalMcts::operator()(const IBoard *board) const
{
    int status = board->GetStatus();
    if (status == game::Draw)
        return 0.0;
    if (status != game::Undecided)
    {
        if (board->GetPlayerToMove() == status)
            return std::numeric_limits<double>::max();
        return std::numeric_limits<double>::min();
    }
    auto results = MCTS<IBoard>(*board, 100);
    double max = -1.1;
    for (auto it : results)
        max = std::max(max, it.second.first / it.second.second);
    return max;
}

}