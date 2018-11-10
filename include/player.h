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

#include <array>
#include <functional>
#include <map>
#include <tuple>

constexpr std::size_t nbOfGoalsPerPlayer{2}; ///< The number of goal to reach for each player
constexpr std::size_t nbOfCharactersPerPlayer{6}; ///< The number of characters each player has at the beginning of the game

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
     * This player will act first
     *
     * \param team The team controlled by this player
     */
    explicit Player(PlayerTeam team) :
        Player{team, true}
    {
        // Nothing
    }

    /**
     * Constructor
     *
     * \param team The team controlled by this player
     * \param playFirst Set it to true to make this player the first one
     */
    Player(PlayerTeam team, bool playFirst) :
        m_team{team},
        m_theirTurn{playFirst},
        m_characters{},
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
     * Tell if this player can play
     * \return True if this is this player's turn
     */
    bool isTheirTurn() const
    {
        return m_theirTurn;
    }

    /**
     * Tell if the this player win the game
     * \return True if this player has all their goals occupied by
     *         one of their character
     */
    bool hasWon() const
    {
        return m_won;
    }

    /**
     * Add a character to this player
     *
     * \param character The character to add to this player
     * \return A pointer to this new character
     */
    Character* addCharacter(Character&& character)
    {
        auto res{m_characters.emplace(character.getPosition().y, character)};

        return res.second ? &res.first->second : nullptr;
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
     * Tell if a given character is on a goal
     *
     * \param c The character to test
     * \return True if the character is on a goal
     */
    bool isOnAGoal(const Character& c) const;

private:
    /**
     * Position comparator
     *
     * Order positions by ascending x, then by ascending y
     */
    struct PositionComp {
        /**
         * Compare the two positions
         *
         * \param lhs The first position
         * \param rhs The second position
         * \return True if the first position is "smaller" than the second one
         */
        bool operator()(const gf::Vector2i& lhs, const gf::Vector2i& rhs) const
        {
            return std::tie(lhs.x, lhs.y) < std::tie(rhs.x, rhs.y);
        }
    };

    PlayerTeam m_team; ///< The team controlled by this player

    bool m_theirTurn{true}; ///< Can the player play?
    bool m_won{false}; ///< Has the player won?

    std::map<int,Character> m_characters; ///< The characters controlled by this player
    std::array<Goal, nbOfGoalsPerPlayer> m_goals; ///< The goals this player has to reach
};

#endif // PLAYER_H
