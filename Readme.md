[![Build Status](https://travis-ci.org/kaspersky/gengine.png)](https://travis-ci.org/kaspersky/gengine)

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
// The winning player ID, i.e. 1 for Player1, 2 for Player2, etc.
int GetStatus() const;

IPlayer GetPlayerToMove() const;
std::size_t Hash() const;
```

# IBot interface

The IBot base class contains logic about how a game is to be player. Any custom bot has to implement the following:

```C++
template <typename IGame>
class IBot
{
protected:
    // This will be provisioned to the bot in the initialization
    IGame *game;

public:
    // Constructor, default implementation provided.
    IBot(const IGame *game);

    // Destructor, default implementation provided.
    virtual ~IBot();

    // Used to make a copy of the bot.
    virtual IBot *Clone() const = 0;

    // Requires the bot to choose (and apply) a move.
    virtual IMove MakeMove() = 0;

    // Informs the bot that the opponent made a move.
    virtual void SendMove(const IMove &move);
};
```

# Generic bots

Several generic bot implementations are already provided, for convenience:
- generic::RandomBot
- generic::MinimaxBot
- generic::ABetaBot
- generic::FixedMCTSBot