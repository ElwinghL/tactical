/**
 * A file defining the character class
 * \author Fabien Matusalem
 */
#ifndef CHARACTER_H
#define CHARACTER_H

#include "utility.h"

#include <gf/Array2D.h>
#include <gf/Orientation.h>
#include <gf/Vector.h>
#include <gf/VectorOps.h>

#include <iostream>
#include <optional>
#include <set>
#include <vector>

class Action;

/**
 * Represent a playable character
 */
class Character {
public:
    /**
     * Constructor
     *
     * \param team The character's team : Cthulhu or Satan
     * \param type The character's type : Tank, Suppport, Scout
     */
    Character(PlayerTeam team, CharacterType type) :
        m_team{team},
        m_type{type},
        m_hp{getHPMaxForType(type)}
    {
        //Nothing
    }

    /**
     * Team getter
     * \return The team of this character
     */
    PlayerTeam getTeam() const
    {
        return m_team;
    }

    /**
     * Type getter
     * \return The type of this character
     */
    CharacterType getType() const
    {
        return m_type;
    }

    /**
     * Maximum HP getter
     * \return The maximum HP this character can get,
     *         which depends of the type
     */
    int getHPMax() const
    {
        return getHPMaxForType(m_type);
    }

    /**
     * HP getter
     * \return The current HP of this character
     */
    int getHP() const
    {
        return m_hp;
    }

    /**
     * Tell if the character is dead
     * \return True if the character's HP is 0 (or below), false otherwise
     */
    bool isDead() const
    {
        return m_hp <= 0;
    }

    /**
     * Give an amount of damage to this character
     * \param amount The amount of HP to take from this character.
     *               Can't be negative.
     */
    void damage(int amount)
    {
        if (amount > 0) {
            m_hp = (amount > m_hp) ? 0 : m_hp - amount;
        }
    }

    void attack(Character& other) const
    {
        other.damage(getDamageForType(m_type));
    }

    static constexpr int getGlobalHPMax()
    {
        return getHPMaxForType(CharacterType::Tank);
    }

    bool operator==(const Character& other) const
    {
        return std::tie(m_team, m_type, m_hp) == std::tie(other.m_team, other.m_type, other.m_hp);
    }

private:
    /**
     * Give the maximum amount of HP according to the type of character
     *
     * \param type The type of character
     * \return The maximum number of of HP for this type
     */
    static constexpr int getHPMaxForType(CharacterType type)
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

    /**
     * Give the damage dealt according to the type of character
     *
     * \param type The type of character
     * \return The amount of damage this type of character deal
     */
    static constexpr int getDamageForType(CharacterType type)
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

    PlayerTeam m_team; ///< The team this character belongs to
    CharacterType m_type; ///< This character's type

    int m_hp; ///< This character's current HP
};

#endif // CHARACTER_H
