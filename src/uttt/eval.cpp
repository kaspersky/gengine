#include <algorithm>
#include <limits>

#include <mcts.h>

#include "uttt.h"
#include "utttp.h"

static double g_eval_micro1[19683];
static double g_eval_macro1[262144];
static double g_micro_double_bonus1 = 2.0;
static double g_macro_double_bonus1 = 15.0;
static double g_eval_micro_matrix1[3][3] = {{3.0, 1.0, 3.0}, {1.0, 4.0, 1.0}, {3.0, 1.0, 3.0}};
static double g_eval_macro_matrix1[3][3] = {{20.0, 10.0, 20.0}, {10.0, 30.0, 10.0}, {20.0, 10.0, 20.0}};
static double g_eval_micro_relative[9] = {2.0, 1.0, 2.0, 1.0, 3.0, 1.0, 2.0, 1.0, 2.0};

void
InitEval1()
{
    for (short i = 0; i < 19683; i++)
    {
        double points = 0.0;
        auto board = makeBoard(i);
        if (g_board_status[i] == 0)
        {
            for (int u = 0; u < 3; ++u)
                for (int v = 0; v < 3; ++v)
                    if (board[u][v] != 0)
                        points += (-2 * board[u][v] + 3) * g_eval_micro_matrix1[u][v];
            for (int u = 0; u < 3; ++u)
                for (int v = 0; v < 2; ++v)
                {
                    if (board[u][v] == board[u][v + 1] && board[u][v] != 0)
                        points += (-2 * board[u][v] + 3) * g_micro_double_bonus1;
                    if (board[v][u] == board[v + 1][u] && board[v][u] != 0)
                        points += (-2 * board[v][u] + 3) * g_micro_double_bonus1;
                }
            if (board[1][1] != 0)
            {
                if (board[1][1] == board[0][0] || board[1][1] == board[2][2])
                    points += (-2 * board[1][1] + 3) * g_micro_double_bonus1;
                if (board[1][1] == board[2][0] || board[1][1] == board[0][2])
                    points += (-2 * board[1][1] + 3) * g_micro_double_bonus1;
            }
        }
        g_eval_micro1[i] = points;
    }

    for (int i = 0; i < 262144; i++)
    {
        double points = 0.0;
        auto board = makeBoard4(i);
        for (int u = 0; u < 3; ++u)
            for (int v = 0; v < 3; ++v)
                if (board[u][v] == 1 || board[u][v] == 2)
                    points += (-2 * board[u][v] + 3) * g_eval_macro_matrix1[u][v];
        for (int u = 0; u < 3; ++u)
            for (int v = 0; v < 2; ++v)
            {
                if (board[u][v] == board[u][v + 1] && board[u][v] != 0)
                    points += (-2 * board[u][v] + 3) * g_macro_double_bonus1;
                if (board[v][u] == board[v + 1][u] && board[v][u] != 0)
                    points += (-2 * board[v][u] + 3) * g_macro_double_bonus1;
            }
        if (board[1][1] == 1 || board[1][1] == 2)
        {
            if (board[1][1] == board[0][0] || board[1][1] == board[2][2])
                points += (-2 * board[1][1] + 3) * g_macro_double_bonus1;
            if (board[1][1] == board[2][0] || board[1][1] == board[0][2])
                points += (-2 * board[1][1] + 3) * g_macro_double_bonus1;
        }
        g_eval_macro1[i] = points;
    }
}

namespace uttt {

double
Eval1::operator()(const IBoard &board) const
{
    int status = board.GetStatus();
    if (status == game::Draw)
        return 0.0;
    if (status != game::Undecided)
    {
        if (board.GetPlayerToMove() == status)
            return std::numeric_limits<double>::max();
        return -std::numeric_limits<double>::max();
    }
    double points = 0.0;
    for (int i = 0; i < 9; ++i)
        points += g_eval_micro1[board.micro[i]] * g_eval_micro_relative[i];
    points += g_eval_macro1[board.macro];
    return (-2 * board.player + 3) * points;
}

double
EvalMcts::operator()(const IBoard &board) const
{
    int status = board.GetStatus();
    if (status == game::Draw)
        return 0.0;
    if (status != game::Undecided)
    {
        if (board.GetPlayerToMove() == status)
            return std::numeric_limits<double>::max();
        return -std::numeric_limits<double>::max();
    }
    auto results = mcts::MCTS_parallel<IBoard>(board, 100);
    double max = -1.1;
    for (const auto &result : results)
        max = std::max(max, result.value / result.total);
    return max;
}

}