#ifndef CHARACTER_H
#define CHARACTER_H

#include "utility.h"

#include <gf/Vector.h>

#include <cassert>

class Character {
public:
    Character(PlayerTeam team, CharacterType type, const gf::Vector2i& pos) :
        m_team{team},
        m_type{type},
        m_pos{pos},
        m_hpMax{getHPMaxForType(type)}
    {
        assert(positionIsValid(pos));
    }

    PlayerTeam getTeam() const
    {
        return m_team;
    }

    CharacterType getType() const
    {
        return m_type;
    }

    gf::Vector2i getPosition() const
    {
        return m_pos;
    }

    int getHPMax() const
    {
        return m_hpMax;
    }

    int getHP() const
    {
        return m_hp;
    }

    bool isDead() const
    {
        return m_hp > 0;
    }

    void damage(int amount)
    {
        assert(amount >= 0);

        m_hp = (amount > m_hp) ? 0 : m_hp - amount;
    }

    bool canAttack(Character& other) const
    {
        return m_team != other.m_team;
    }

    bool attack(Character& other) const
    {
        bool success{canAttack(other)};

        if (success) {
            other.damage(getDamageForType(m_type));
        }

        return success;
    }

    bool move(const gf::Vector2i& pos);
    bool canMove(const gf::Vector2i& pos) const;

private:
    static int getHPMaxForType(CharacterType type)
    {
        switch (type) {
        case CharacterType::Tank:
            return 8;
        case CharacterType::Support:
            return 5;
        case CharacterType::Scout:
            return 3;
        }

        return -1;
    }

    static int getDamageForType(CharacterType type)
    {
        switch (type) {
        case CharacterType::Tank:
            return 2;
        case CharacterType::Support:
            return 2;
        case CharacterType::Scout:
            return 1;
        }

        return -1;
    }

    PlayerTeam m_team;
    CharacterType m_type;

    gf::Vector2i m_pos;

    int m_hpMax;
    int m_hp{m_hpMax};
};

#endif // CHARACTER_H
