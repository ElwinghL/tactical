/**
 * A file defining the players' goals, which are useful to win
 * \author Fabien Matusalem
 */
#ifndef GOAL_H
#define GOAL_H

#include "utility.h"

#include <gf/Vector.h>

/**
 * A player's goal
 *
 * All goals must be occupied by a character to win the game
 */
class Goal {
public:
    /**
     * Constructor
     *
     * \param team The team which has to occupy this goal
     * \param pos The position of this goal
     */
    Goal(PlayerTeam team, const gf::Vector2i& pos) :
        m_team{team},
        m_pos{pos}
    {
        // Nothing
    }

    /**
     * Team getter
     * \return The team which has to occupy this goal
     */
    PlayerTeam getTeam() const
    {
        return m_team;
    }

    /**
     * Postion getter
     * \return The position of this goal
     */
    gf::Vector2i getPosition() const
    {
        return m_pos;
    }

private:
    PlayerTeam m_team; ///< The team aiming to this goal
    gf::Vector2i m_pos; ///< This goal's position
};

#endif // GOAL_H
