/**
 * A file defining the game's AI
 * \author Fabien Matusalem
 */
#ifndef GAMEAI_H
#define GAMEAI_H

#include "player.h"

/**
 * The artificial intelligence class
 *
 * This AI is based on the alpha-beta algorithm
 */
class GameAI : public Player {
public:
    /**
     * Constructor
     * \param team The team the AI controls
     */
    explicit GameAI(PlayerTeam team) :
        Player{team, false}
    {
        // Nothing
    }

    /**
     * Simulate the actions
     */
    void simulateActions();

    void playTurn(gf::Array2D<boost::optional<Character>, int>& board);

private:
};

#endif // GAMEAI_H
