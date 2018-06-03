#pragma once

typedef std::array<std::array<int, 3>, 3> TBoard;

extern char g_board_status[19683];
extern char g_macro_board_status[262144];

void InitEval1();

TBoard makeBoard(short k);

TBoard makeBoard4(int k);

int TBoardToInt4(const TBoard &board);