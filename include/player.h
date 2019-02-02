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

constexpr std::size_t nbOfGoalsPerPlayer{2}; ///< The number of goal to reach for each player

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
    Player(PlayerTeam team) :
        m_team{team},
        m_goals{Goal{m_team, {0, 0}}, Goal{m_team, {0, 0}}}
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

    /**
     * Goals getter
     * \return An array containing the goals
     */
    std::array<Goal, 2>& getGoals()
    {
        return m_goals;
    }

    /**
     * Tell if the this player win the game
     * \return True if this player has all their goals occupied by
     *         one of their character
     */
    bool hasWon() const
    {
        return std::all_of(m_goals.cbegin(), m_goals.cend(), [](const Goal& goal) {
            return goal.isActivated();
        });
    }

    /**
     * Change the positions of this player's goals
     * \param positions An array containing the positions for all goals
     */
    void setGoalPositions(const std::array<gf::Vector2i, nbOfGoalsPerPlayer>& positions)
    {
        for (std::size_t i{0}; i < nbOfGoalsPerPlayer; ++i) {
            m_goals[i] = Goal{m_team, positions[i]};
        }
    }

    /**
     * Activate goals if a character of this player is on top
     */
    void activateGoals();

protected:
    PlayerTeam m_team; ///< The team controlled by this player


    std::array<Goal, nbOfGoalsPerPlayer> m_goals; ///< The goals this player has to reach
};

#endif // PLAYER_H
