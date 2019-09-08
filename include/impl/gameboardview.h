#ifndef IMPL_GAMEBOARDVIEW_H
#define IMPL_GAMEBOARDVIEW_H

constexpr int getPriorityFromPos(const gf::Vector2i& pos)
{
    return pos.y - pos.x - 6; // TODO magic constant
}

constexpr int getPriorityFromPos(const gf::Vector2f& pos)
{
    return static_cast<int>(pos.y - pos.x) - 6;
}

[[nodiscard]] constexpr std::size_t GameboardView::getActivationIndex(bool activated) const
{
    return activated ? 1 : 0;
}

inline GameboardView::EntityCharacter::EntityCharacter(GameboardView& gbView, gf::Sprite sprite, int hp, const gf::Vector2i& pos):
    Entity{getPriorityFromPos(pos)},
    m_gbView{&gbView},
    m_sprite{std::move(sprite)},
    m_origin{static_cast<float>(pos.x), static_cast<float>(pos.y)},
    m_dest{m_origin},
    m_timeFrac{1.0f},
    m_currentHPSprite{checkHP(hp)}
{
    // Nothing
}

inline void GameboardView::EntityCharacter::setHP(int hp)
{
    m_currentHPSprite = checkHP(hp);
}

inline void GameboardView::EntityCharacter::setLocked(bool locked)
{
    m_locked = locked;
}

[[nodiscard]] inline bool GameboardView::EntityCharacter::animationFinished() const
{
    return gf::almostEquals(m_timeFrac, 1.0f);
}

[[nodiscard]] inline std::size_t GameboardView::EntityCharacter::checkHP(int hp)
{
    assert(hp > 0);
    return static_cast<std::size_t>(hp - 1);
}


#endif // IMPL_GAMEBOARDVIEW_H
