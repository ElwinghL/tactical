#ifndef IMPL_HUMANPLAYER_H
#define IMPL_HUMANPLAYER_H

constexpr HumanPlayer::HumanPlayer(PlayerTeam team):
    Player{team}
{
    // Nothing
}

[[nodiscard]] constexpr bool HumanPlayer::hasMoved() const
{
    return m_hasMoved;
}

constexpr void HumanPlayer::setMoved(bool newValue)
{
    m_hasMoved = newValue;
}

#endif //IMPL_HUMANPLAYER_H
