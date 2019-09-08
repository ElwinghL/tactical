#ifndef IMPL_GOAL_H
#define IMPL_GOAL_H

constexpr Goal::Goal(PlayerTeam team, const gf::Vector2i& pos) :
    m_team{team},
    m_pos{pos}
{
    // Nothing
}

/**
 * Team getter
 * \return The team which has to occupy this goal
 */
[[nodiscard]] constexpr PlayerTeam Goal::getTeam() const
{
    return m_team;
}

/**
 * Postion getter
 * \return The position of this goal
 */
[[nodiscard]] constexpr gf::Vector2i Goal::getPosition() const
{
    return m_pos;
}

/**
 * Activate this goal
 */
constexpr void Goal::activate()
{
    m_activated = true;
}

/**
 * Is this goal activated?
 * \return true if this goal has been activated
 *         by the correct team
 */
[[nodiscard]] constexpr bool Goal::isActivated() const
{
    return m_activated;
}

constexpr bool Goal::operator==(const Goal& other) const
{
    return std::tie(m_team, m_pos, m_activated) == std::tie(other.m_team, other.m_pos, other.m_activated);
}

#endif // IMPL_GOAL_H
