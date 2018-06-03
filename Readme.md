Build: [![Build Status](https://travis-ci.org/kaspersky/gengine.png)](https://travis-ci.org/kaspersky/gengine)

# Introduction

# IGame interface

The IGame template represents a game state. It has to provide the following:

```C++
// Copy constructor
IGame(const IGame &other);

bool operator==(const IGame &other) const;
void ApplyMove(const IMove &move);
std::vector<IMove> GetPossibleMoves() const;
int GetMoveCount() const;

// By convention, the return value should be:
// game::Undecided, for an unfinished game
// game::Draw, for a draw
// The winning player ID
int GetStatus() const;

IPlayer GetPlayerToMove() const;
std::size_t Hash() const;