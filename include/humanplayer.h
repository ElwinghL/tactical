/**
 * A class used for a human player, but not if the player is an AI
 * \author Simon Gigant
 */
#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include "player.h"

class HumanPlayer : public Player {
public:
    /**
     * Constructor
     * \param team The team the human controls
     */
    explicit constexpr HumanPlayer(PlayerTeam team);

    [[nodiscard]] constexpr bool hasMoved() const;
    constexpr void setMoved(bool newValue);


private:
    bool m_hasMoved{false};
};

#include "impl/humanplayer.h"

#endif // HUMANPLAYER_H
