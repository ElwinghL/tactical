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
    constexpr explicit Goal(PlayerTeam team, const gf::Vector2i& pos);

    /**
     * Team getter
     * \return The team which has to occupy this goal
     */
    [[nodiscard]] constexpr PlayerTeam getTeam() const;

    /**
     * Postion getter
     * \return The position of this goal
     */
    [[nodiscard]] constexpr gf::Vector2i getPosition() const;

    /**
     * Activate this goal
     */
    constexpr void activate();

    /**
     * Is this goal activated?
     * \return true if this goal has been activated
     *         by the correct team
     */
    [[nodiscard]] constexpr bool isActivated() const;

    constexpr bool operator==(const Goal& other) const;

private:
    PlayerTeam m_team; ///< The team aiming to this goal
    gf::Vector2i m_pos; ///< This goal's position
    bool m_activated{false};
};

#include "impl/goal.h"

#endif // GOAL_H
