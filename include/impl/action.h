#ifndef IMPL_ACTION_H
#define IMPL_ACTION_H

constexpr Action::Action(ActionType type, const gf::Vector2i& origin, const gf::Vector2i& dest, const gf::Vector2i& target) :
    m_type{type},
    m_origin{origin},
    m_dest{dest},
    m_target{target}
{
    // Nothing
}

constexpr Action::Action(const gf::Vector2i& origin, const gf::Vector2i& dest) :
    Action{ActionType::None, origin, dest, origin}
{
    // Nothing
}

constexpr Action::Action(ActionType type, const gf::Vector2i& origin, const gf::Vector2i& target) :
    Action{type, origin, origin, target}
{
    // Nothing
}

[[nodiscard]] constexpr gf::Vector2i Action::getDest() const
{
    return m_dest;
}

[[nodiscard]] constexpr gf::Vector2i Action::getTarget() const
{
    return m_target;
}

[[nodiscard]] constexpr ActionType Action::getType() const
{
    return m_type;
}

[[nodiscard]] constexpr gf::Vector2i Action::getOrigin() const
{
    return m_origin;
}

[[nodiscard]] constexpr gf::Vector2i Action::getMove() const
{
    return m_dest - m_origin;
}

#endif //IMPL_ACTION_H
