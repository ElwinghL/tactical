/**
 * A class used for a human player, but not if the player is an AI
 * \author Simon Gigant
 */
#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include "player.h"

/**
 * 
 *
 * 
 */
class HumanPlayer : public Player {
public:
    /**
     * Constructor
     * \param team The team the human controls
     */
    explicit HumanPlayer(PlayerTeam team) :
        Player{team}
    {
        // Nothing
    }

    bool hasMoved() const
    {
        return m_hasMoved;
    }

    void setMoved(bool newValue)
    {
        m_hasMoved = newValue;
    }


private:
    bool m_hasMoved{false};
};

#endif // HUMANPLAYER_H
