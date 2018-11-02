#ifndef GAMEAI_H
#define GAMEAI_H

#include "player.h"

class GameAI : public Player {
public:
    explicit GameAI(PlayerTeam team) :
        Player{team, false, false}
    {
        // Nothing
    }

    void simulateActions();

private:
};

#endif // GAMEAI_H
