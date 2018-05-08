#include <iostream>
#include <vector>
#include <array>

#include <uttt.h>

namespace uttt {

static short g_factors[9] = {1, 3, 9, 27, 81, 243, 729, 2187, 6561};
static int g_factors4[9] = {1, 4, 16, 64, 256, 1024, 4096, 16384, 65536};
static char g_board_status[19683];
static int g_board_value[2][19683];
static std::vector<char> g_moves[19683];
static char g_macro_board_status[262144];

static void
makeBoard(char board[3][3], short k)
{
    board[0][0] = (k / g_factors[0]) % 3;
    board[0][1] = (k / g_factors[1]) % 3;
    board[0][2] = (k / g_factors[2]) % 3;
    board[1][0] = (k / g_factors[3]) % 3;
    board[1][1] = (k / g_factors[4]) % 3;
    board[1][2] = (k / g_factors[5]) % 3;
    board[2][0] = (k / g_factors[6]) % 3;
    board[2][1] = (k / g_factors[7]) % 3;
    board[2][2] = (k / g_factors[8]) % 3;
}

static void
makeBoard4(char board[3][3], int k)
{
    board[0][0] = (k / g_factors4[0]) % 4;
    board[0][1] = (k / g_factors4[1]) % 4;
    board[0][2] = (k / g_factors4[2]) % 4;
    board[1][0] = (k / g_factors4[3]) % 4;
    board[1][1] = (k / g_factors4[4]) % 4;
    board[1][2] = (k / g_factors4[5]) % 4;
    board[2][0] = (k / g_factors4[6]) % 4;
    board[2][1] = (k / g_factors4[7]) % 4;
    board[2][2] = (k / g_factors4[8]) % 4;
}

static char
getBoardStatus(const char board[3][3])
{
    for (int i = 0; i < 3; i++)
    {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != 0)
            return (board[i][0] == 1 ? IGame::Win1 : IGame::Win2);
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != 0)
            return (board[0][i] == 1 ? IGame::Win1 : IGame::Win2);
    }
    if ((board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != 0) ||
        (board[2][0] == board[1][1] && board[1][1] == board[0][2] && board[2][0] != 0))
        return (board[1][1] == 1 ? IGame::Win1 : IGame::Win2);
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (board[i][j] == 0)
                return IGame::Undecided;
    return IGame::Draw;
}

static char
getMacroBoardStatus(const char board[3][3])
{
    for (int i = 0; i < 3; i++)
    {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != 0 && board[i][0] != 3)
            return (board[i][0] == 1 ? IGame::Win1 : IGame::Win2);
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != 0 && board[0][i] != 3)
            return (board[0][i] == 1 ? IGame::Win1 : IGame::Win2);
    }
    if ((board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != 0 && board[0][0] != 3) ||
        (board[2][0] == board[1][1] && board[1][1] == board[0][2] && board[2][0] != 0 && board[2][0] != 3))
        return (board[1][1] == 1 ? IGame::Win1 : IGame::Win2);
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (board[i][j] == 0)
                return IGame::Undecided;
    return IGame::Draw;
}

static int
getBoardValue(char board[3][3], char player, char board_status)
{
    if (board_status != IGame::Undecided)
        return 0;

    int value = 0;

    if (board[1][1] == player)
        value += 3;
    else if (board[1][1] == 3 - player)
        value -= 3;

    if (board[1][1] == player && (board[1][1] == board[0][0] || board[1][1] == board[0][2] || board[1][1] == board[2][0] || board[1][1] == board[2][2] || board[1][1] == board[0][1] || board[1][1] == board[1][0] || board[1][1] == board[1][2] || board[1][1] == board[2][1]))
        value += 2;
    if (board[1][1] == 3 - player && (board[1][1] == board[0][0] || board[1][1] == board[0][2] || board[1][1] == board[2][0] || board[1][1] == board[2][2] || board[1][1] == board[0][1] || board[1][1] == board[1][0] || board[1][1] == board[1][2] || board[1][1] == board[2][1]))
        value -= 2;
    if (board[0][1] == player && (board[0][1] == board[0][0] || board[0][1] == board[0][2]))
        value += 2;
    if (board[0][1] == 3 - player && (board[0][1] == board[0][0] || board[0][1] == board[0][2]))
        value -= 2;
    if (board[1][0] == player && (board[1][0] == board[0][0] || board[1][0] == board[2][0]))
        value += 2;
    if (board[1][0] == 3 - player && (board[1][0] == board[0][0] || board[1][0] == board[2][0]))
        value -= 3;
    if (board[1][2] == player && (board[1][2] == board[0][2] || board[1][2] == board[2][2]))
        value += 2;
    if (board[1][2] == 3 - player && (board[1][2] == board[0][2] || board[1][2] == board[2][2]))
        value -= 2;
    if (board[2][1] == player && (board[2][1] == board[2][0] || board[2][1] == board[2][2]))
        value += 2;
    if (board[2][1] == 3 - player && (board[2][1] == board[2][0] || board[2][1] == board[2][2]))
        value -= 2;

    return value;
}

struct UtttInit
{
    UtttInit()
    {
        char board[3][3];
        for (short i = 0; i < 19683; i++)
        {
            makeBoard(board, i);
            g_board_status[i] = getBoardStatus(board);
            g_board_value[0][i] = getBoardValue(board, 1, g_board_status[i]);
            g_board_value[1][i] = -g_board_value[0][i];
        }

        for (short i = 0; i < 19683; i++)
        {
            makeBoard(board, i);
        }

        for (short i = 0; i < 19683; i++)
            if (g_board_status[i] == IGame::Undecided)
            {
                //for (int j = 0; j < 9; j++)
                //    if ((i / g_factors[j]) % 3 == 0)
                //        g_moves[i].push_back(j);
                if ((i / g_factors[4]) % 3 == 0)
                    g_moves[i].push_back(4);
                if ((i / g_factors[0]) % 3 == 0)
                    g_moves[i].push_back(0);
                if ((i / g_factors[2]) % 3 == 0)
                    g_moves[i].push_back(2);
                if ((i / g_factors[6]) % 3 == 0)
                    g_moves[i].push_back(6);
                if ((i / g_factors[8]) % 3 == 0)
                    g_moves[i].push_back(8);
                if ((i / g_factors[1]) % 3 == 0)
                    g_moves[i].push_back(1);
                if ((i / g_factors[3]) % 3 == 0)
                    g_moves[i].push_back(3);
                if ((i / g_factors[5]) % 3 == 0)
                    g_moves[i].push_back(5);
                if ((i / g_factors[7]) % 3 == 0)
                    g_moves[i].push_back(7);
            }

        for (int i = 0; i < 262144; i++)
        {
            makeBoard4(board, i);
            g_macro_board_status[i] = getMacroBoardStatus(board);
        }
    }
};

static UtttInit g_UtttInit;

IBoard::IBoard(int macro, const std::array<short, 9> &micro, char next, char player): macro(macro), micro(micro), next(next), player(player)
{
    this->player = player;
}

IBoard::IBoard(): macro(0), micro({}), next(-1), player(1)
{
}

bool
IBoard::operator==(const IBoard &other) const
{
    return macro == other.macro && micro == other.micro && next == other.next;
}

void
IBoard::Print() const
{
    std::cout << "Player to move: " << static_cast<int>(player) << '\n';
    std::cout << static_cast<int>(next) << '\n';
    int data1[3][3];
    for (int i = 0; i < 9; i++)
        data1[i / 3][i % 3] =  macro / g_factors4[i] % 4;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
            std::cout << data1[i][j];
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
    std::cout << "Status: " << static_cast<int>(GetStatus()) << "\n\n";
}

void
IBoard::computeMacro()
{
    macro = g_board_status[micro[0]] * g_factors4[0] + g_board_status[micro[1]] * g_factors4[1] + g_board_status[micro[2]] * g_factors4[2] + g_board_status[micro[3]] * g_factors4[3] + g_board_status[micro[4]] * g_factors4[4] + g_board_status[micro[5]] * g_factors4[5] + g_board_status[micro[6]] * g_factors4[6] + g_board_status[micro[7]] * g_factors4[7] + g_board_status[micro[8]] * g_factors4[8];
}

std::vector<IMove>
IBoard::GetPossibleMoves() const
{
    std::vector<IMove> moves;
    if (next >= 0)
        for (auto m : g_moves[micro[next]])
            moves.emplace_back(next * 9 + m);
    else
        for (int i = 0; i < 9; i++)
            if (g_board_status[micro[i]] == Undecided)
                for (auto m : g_moves[micro[i]])
                    moves.emplace_back(i * 9 + m);
    return moves;
}

void
IBoard::Move(const IMove &move)
{
    micro[move / 9] += player * g_factors[move % 9];
    macro += g_board_status[micro[move / 9]] * g_factors4[move / 9];
    if (g_board_status[micro[move % 9]] == Undecided)
        next = move % 9;
    else
        next = -1;
    player = 3 - player;
}

void
IBoard::updateMicro(const std::array<char, 81> &other)
{
    micro[0] = other[0] * g_factors[0] + other[1] * g_factors[1] + other[2] * g_factors[2] + other[9] * g_factors[3] + other[10] * g_factors[4] + other[11] * g_factors[5] + other[18] * g_factors[6] + other[19] * g_factors[7] + other[20] * g_factors[8];
    micro[1] = other[3] * g_factors[0] + other[4] * g_factors[1] + other[5] * g_factors[2] + other[12] * g_factors[3] + other[13] * g_factors[4] + other[14] * g_factors[5] + other[21] * g_factors[6] + other[22] * g_factors[7] + other[23] * g_factors[8];
    micro[2] = other[6] * g_factors[0] + other[7] * g_factors[1] + other[8] * g_factors[2] + other[15] * g_factors[3] + other[16] * g_factors[4] + other[17] * g_factors[5] + other[24] * g_factors[6] + other[25] * g_factors[7] + other[26] * g_factors[8];
    micro[3] = other[27] * g_factors[0] + other[28] * g_factors[1] + other[29] * g_factors[2] + other[36] * g_factors[3] + other[37] * g_factors[4] + other[38] * g_factors[5] + other[45] * g_factors[6] + other[46] * g_factors[7] + other[47] * g_factors[8];
    micro[4] = other[30] * g_factors[0] + other[31] * g_factors[1] + other[32] * g_factors[2] + other[39] * g_factors[3] + other[40] * g_factors[4] + other[41] * g_factors[5] + other[48] * g_factors[6] + other[49] * g_factors[7] + other[50] * g_factors[8];
    micro[5] = other[33] * g_factors[0] + other[34] * g_factors[1] + other[35] * g_factors[2] + other[42] * g_factors[3] + other[43] * g_factors[4] + other[44] * g_factors[5] + other[51] * g_factors[6] + other[52] * g_factors[7] + other[53] * g_factors[8];
    micro[6] = other[54] * g_factors[0] + other[55] * g_factors[1] + other[56] * g_factors[2] + other[63] * g_factors[3] + other[64] * g_factors[4] + other[65] * g_factors[5] + other[72] * g_factors[6] + other[73] * g_factors[7] + other[74] * g_factors[8];
    micro[7] = other[57] * g_factors[0] + other[58] * g_factors[1] + other[59] * g_factors[2] + other[66] * g_factors[3] + other[67] * g_factors[4] + other[68] * g_factors[5] + other[75] * g_factors[6] + other[76] * g_factors[7] + other[77] * g_factors[8];
    micro[8] = other[60] * g_factors[0] + other[61] * g_factors[1] + other[62] * g_factors[2] + other[69] * g_factors[3] + other[70] * g_factors[4] + other[71] * g_factors[5] + other[78] * g_factors[6] + other[79] * g_factors[7] + other[80] * g_factors[8];
}

int
IBoard::GetStatus() const
{
    return g_macro_board_status[macro];
}

IGame *
IBoard::Clone() const
{
    return new IBoard(macro, micro, next, player);
}

std::size_t
IBoard::Hash() const
{
    std::size_t ret = macro;
    for (auto n : micro)
        ret = (ret << 1) ^ n;
    return (ret << 1) ^ next;
}

bool
IBoard::Equal(const IGame *game) const
{
    const IBoard *iboard = dynamic_cast<const IBoard *>(game);
    return *this == *iboard;
}

} // namespace utttce std
