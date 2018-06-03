#include <random>
#include <iostream>

#include "ttt.h"

static int
GetStatus(const ttt::TBoard &board)
{

    for (int i = 0; i < 3; ++i)
    {
        if (board[i][1] == board[i][0] && board[i][1] == board[i][2] && board[i][1] != 0)
            return board[i][1];
        if (board[1][i] == board[0][i] && board[1][i] == board[2][i] && board[1][i] != 0)
            return board[1][i];
    }
    if ((board[1][1] == board[0][0] && board[1][1] == board[2][2] && board[1][1] != 0) || (board[1][1] == board[0][2] && board[1][1] == board[2][0] && board[1][1] != 0))
        return board[1][1];
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            if (board[i][j] == 0)
                return game::Undecided;
    return game::Draw;
}

namespace ttt {

Board::Board(): board{}, player(1)
{
}

bool
Board::operator==(const Board &other) const
{
    if (player != other.player)
        return false;
    if (board == other.board)
        return true;
    TBoard tboard(board);
    std::swap(tboard[0][0], tboard[0][2]);
    std::swap(tboard[1][0], tboard[1][2]);
    std::swap(tboard[2][0], tboard[2][2]);
    if (tboard == other.board)
        return true;
    tboard = TBoard(board);
    std::swap(tboard[0][0], tboard[2][0]);
    std::swap(tboard[0][1], tboard[2][1]);
    std::swap(tboard[0][2], tboard[2][2]);
    if (tboard == other.board)
        return true;
    tboard = TBoard(board);
    std::swap(tboard[0][1], tboard[1][0]);
    std::swap(tboard[0][2], tboard[2][0]);
    std::swap(tboard[1][2], tboard[2][1]);
    if (tboard == other.board)
        return true;
    tboard = TBoard(board);
    std::swap(tboard[0][1], tboard[1][2]);
    std::swap(tboard[1][0], tboard[2][1]);
    std::swap(tboard[0][0], tboard[2][2]);
    return tboard == other.board;
}

int
Board::GetStatus() const
{
    return ::GetStatus(board);
}

void
Board::ApplyMove(const game::IMove &move)
{
    board[move / 3][move % 3] = player;
    player = 3 - player;
}

std::vector<game::IMove>
Board::GetPossibleMoves() const
{
    std::vector<game::IMove> moves;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            if (board[i][j] == 0)
                moves.push_back(3 * i + j);
    return moves;
}

int
Board::GetMoveCount() const
{
    return GetPossibleMoves().size();
}

game::IPlayer
Board::GetPlayerToMove() const
{
    return player;
}

std::size_t
Board::Hash() const
{
    std::size_t result = 0;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            result += board[i][j];
    return (result << 1) ^ player;
}

void
Board::Print() const
{
    std::cout << "Player to move: " << static_cast<int>(player) << '\n';
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
            std::cout << static_cast<int>(board[i][j]) << ' ';
        std::cout << '\n';
    }
    std::cout << "Status: " << GetStatus() << "\n\n";
}

}