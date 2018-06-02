#include <mcts.h>
#include "uttt.h"
#include "utttp.h"

static bool g_is_definitive_draw[262144];

void
InitBot()
{
    for (int i = 0; i < 262144; ++i)
    {
        g_is_definitive_draw[i] = false;
        auto board = makeBoard4(i);
        for (int u = 0; u < 3; ++u)
            for (int v = 0; v < 3; ++v)
                if (board[u][v] == 0)
                    board[u][v] = 1;
        if (g_macro_board_status[TBoardToInt4(board)] != 3)
            continue;
        board = makeBoard4(i);
        for (int u = 0; u < 3; ++u)
            for (int v = 0; v < 3; ++v)
                if (board[u][v] == 0)
                    board[u][v] = 1;
        if (g_macro_board_status[TBoardToInt4(board)] != 3)
            continue;
        g_is_definitive_draw[i] = true;
    }
}

namespace uttt {

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
    auto results = MCTS<IBoard>()(board, mcts_iterations);
    double max = -1.1;
    game::IMove move = -1;
    for (const auto &result : results)
    {
        auto v = result.value / result.total;
        if (v > max)
            max = v, move = result.move;
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

int
RandomPlayout::operator()(const uttt::IBoard &game) const
{
    auto ngame(game);
    int status = ngame.GetStatus();
    while (status == game::Undecided)
    {
        if (g_is_definitive_draw[ngame.macro])
            return game::Draw;
        auto m = ngame.GetRandomMove();
        ngame.ApplyMove(m);
        status = ngame.GetStatus();
    }
    return status;
}

}