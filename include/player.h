/**
 * A file defining a player, human or not
 * \author Fabien Matusalem
 */
#ifndef PLAYER_H
#define PLAYER_H

#include "character.h"
#include "goal.h"
#include "utility.h"

#include <gf/Vector.h>

#include <algorithm>
#include <array>
#include <functional>
#include <map>
#include <utility>

/**
 * A player, who is controlled by the computer of by a human
 */
class Player {
public:
    /**
     * Deleted default constructor
     */
    Player() = delete;

    /**
     * Deleted copy constructor
     */
    Player(const Player&) = delete;

    /**
     * Default move constructor
     */
    Player(Player&&) = default;

    /**
     * Deleted copy
     */
    Player& operator=(const Player&) = delete;

    /**
     * Default move operation
     */
    Player& operator=(Player&&) = default;


    /**
     * Constructor
     *
     * \param team The team controlled by this player
     */
    explicit Player(PlayerTeam team) :
        m_team{team}
    {
        // Nothing
    }

    /**
     * Default virtual destructor
     */
    virtual ~Player() noexcept = default;

    /**
     * Team getter
     * \return The team this player controls
     */
    PlayerTeam getTeam() const
    {
        return m_team;
    }


private:
    PlayerTeam m_team; ///< The team controlled by this player
};

#endif // PLAYER_H
