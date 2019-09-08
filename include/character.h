/**
 * A file defining the character class
 * \author Fabien Matusalem
 */
#ifndef CHARACTER_H
#define CHARACTER_H

#include "utility.h"

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
    constexpr Character(PlayerTeam team, CharacterType type);

    /**
     * Team getter
     * \return The team of this character
     */
    [[nodiscard]] constexpr PlayerTeam getTeam() const;

    /**
     * Type getter
     * \return The type of this character
     */
    [[nodiscard]] constexpr CharacterType getType() const;

    /**
     * Maximum HP getter
     * \return The maximum HP this character can get,
     *         which depends of the type
     */
    [[nodiscard]] constexpr int getHPMax() const;

    /**
     * HP getter
     * \return The current HP of this character
     */
    [[nodiscard]] constexpr int getHP() const;

    /**
     * Tell if the character is dead
     * \return True if the character's HP is 0 (or below), false otherwise
     */
    [[nodiscard]] constexpr bool isDead() const;

    /**
     * Give an amount of damage to this character
     * \param amount The amount of HP to take from this character.
     *               Can't be negative.
     */
    constexpr void damage(int amount);
    constexpr void attack(Character& other) const;

    constexpr bool operator==(const Character& other) const;

    [[nodiscard]] static constexpr int getGlobalHPMax();

private:
    /**
     * Give the maximum amount of HP according to the type of character
     *
     * \param type The type of character
     * \return The maximum number of of HP for this type
     */
    [[nodiscard]] static constexpr int getHPMaxForType(CharacterType type);

    /**
     * Give the damage dealt according to the type of character
     *
     * \param type The type of character
     * \return The amount of damage this type of character deal
     */
    [[nodiscard]] static constexpr int getDamageForType(CharacterType type);

    PlayerTeam m_team; ///< The team this character belongs to
    CharacterType m_type; ///< This character's type

    int m_hp; ///< This character's current HP
};

#include "impl/character.h"

#endif // CHARACTER_H
