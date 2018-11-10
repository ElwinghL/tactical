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
        Player{team, true}
    {
        // Nothing
    }
    
    /**
     * Constructor
     * \param team The team the human controls
     * \param playFirst Set it to true to make this player the first one
     */
    explicit HumanPlayer(PlayerTeam team, bool playFirst) :
        Player{team,playFirst}
    {
        //Nothing
    }
    
    

private:
    
};

#endif // HUMANPLAYER_H
