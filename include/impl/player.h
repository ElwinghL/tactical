#ifndef IMPL_PLAYER_H
#define IMPL_PLAYER_H

constexpr Player::Player(PlayerTeam team):
    m_team{team}
{
    // Nothing
}

[[nodiscard]] constexpr PlayerTeam Player::getTeam() const
{
    return m_team;
}

#endif //IMPL_PLAYER_H
