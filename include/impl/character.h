#ifndef IMPL_CHARACTER_H
#define IMPL_CHARACTER_H

#include <tuple>

#include <cassert>

constexpr Character::Character(PlayerTeam team, CharacterType type) :
    m_team{team},
    m_type{type},
    m_hp{getHPMaxForType(type)}
{
    //Nothing
}

[[nodiscard]] constexpr PlayerTeam Character::getTeam() const
{
    return m_team;
}

[[nodiscard]] constexpr CharacterType Character::getType() const
{
    return m_type;
}

[[nodiscard]] constexpr int Character::getHPMax() const
{
    return getHPMaxForType(m_type);
}

[[nodiscard]] constexpr int Character::getHP() const
{
    return m_hp;
}

[[nodiscard]] constexpr bool Character::isDead() const
{
    return m_hp <= 0;
}

constexpr void Character::damage(int amount)
{
    assert(amount > 0);
    m_hp = (amount > m_hp) ? 0 : m_hp - amount;
}

constexpr void Character::attack(Character& other) const
{
    other.damage(getDamageForType(m_type));
}

constexpr bool Character::operator==(const Character& other) const
{
    return std::tie(m_team, m_type, m_hp) == std::tie(other.m_team, other.m_type, other.m_hp);
}

[[nodiscard]] constexpr int Character::getGlobalHPMax()
{
    return getHPMaxForType(CharacterType::Tank);
}

[[nodiscard]] constexpr int Character::getHPMaxForType(CharacterType type)
{
    switch (type) {
    case CharacterType::Tank:
        return 8;
    case CharacterType::Support:
        return 5;
    case CharacterType::Scout:
        return 3;
    }

    return -1; // to suppress the "no-return" warning
}

[[nodiscard]] constexpr int Character::getDamageForType(CharacterType type)
{
    switch (type) {
    case CharacterType::Tank:
        return 2;
    case CharacterType::Support:
        return 2;
    case CharacterType::Scout:
        return 1;
    }

    return -1; // to suppress the "no-return" warning
}

#endif //IMPL_CHARACTER_H
