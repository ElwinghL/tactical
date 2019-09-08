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
     * Deleted copy constructor
     */
    Player(const Player&) = delete;

    /**
     * Default move constructor
     */
    constexpr Player(Player&&) = default;

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
    explicit constexpr Player(PlayerTeam team);

    /**
     * Default virtual destructor
     */
    virtual ~Player() noexcept = default;

    /**
     * Team getter
     * \return The team this player controls
     */
    [[nodiscard]] constexpr PlayerTeam getTeam() const;


private:
    PlayerTeam m_team; ///< The team controlled by this player
};

#include "impl/player.h"

#endif // PLAYER_H
