/**
 * A file defining the character class
 * \author Fabien Matusalem
 */
#ifndef CHARACTER_H
#define CHARACTER_H

#include "utility.h"

#include <gf/Array2D.h>
#include <gf/Vector.h>
#include <gf/VectorOps.h>

#include <boost/optional.hpp>
#include <iostream>
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
     * \param pos The character's start position
     */
    Character(PlayerTeam team, CharacterType type, const gf::Vector2i& pos) :
        m_team{team},
        m_type{type},
        m_pos{pos},
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
     * Position getter
     * \return The current position of this character
     */
    gf::Vector2i getPosition() const
    {
        return m_pos;
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
        return m_hp > 0;
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

    /**
     * Tell if the character can attack another one
     *
     * \param other The other character this character is trying to attack
     * \param board The board of the game
     * \param usedForNotPossibleDisplay Used for display purpose only. False by default. If true, does not consider view and if there is  character on the case
     * \return True if the other character may be attacked by this character
     */
    bool canAttack(const Character& other, const Gameboard_t& board, bool usedForNotPossibleDisplay = false) const;

    /**
     * Attack another character
     *
     * The amount of damage dealt to the other character
     * depends of the type of this one
     *
     * \param other The character to attack
     * \param board The board of the game
     * \return True if the attack succeeds
     */
    bool attack(Character& other, const Gameboard_t& board) const;

    /**
     * Get a set of every possible movement for the character
     * \param board The board containing pointers to characters
     * \param usedForNotPossibleDisplay Used for display purpose only. False by default. If true, does not consider view and if there is  character on the case
     * \return The set of possible relative vector movements
     */
    std::set<gf::Vector2i, PositionComp> getAllPossibleMoves(const Gameboard_t& board, bool usedForNotPossibleDisplay = false) const;

    /**
     * Get a set of every possible attack for the character
     * \param board The board containing pointers to characters
     * \param usedForNotPossibleDisplay Used for display purpose only. False by default. If true, does not consider view and if there is  character on the case
     * \return The set of possible relative vector attack
     */
    std::set<gf::Vector2i, PositionComp> getAllPossibleAttacks(const Gameboard_t& board, bool usedForNotPossibleDisplay = false) const;

    /**
     * Get a set of every possible capacity for the character
     * \param board The board containing pointers to characters
     * \param usedForNotPossibleDisplay Used for display purpose only. False by default. If true, does not consider view and if there is  character on the case
     * \return The set of possible relative vector capacity
     */
    std::set<gf::Vector2i, PositionComp> getAllPossibleCapacities(const Gameboard_t& board, bool usedForNotPossibleDisplay = false) const;

    /**
     * Move this character
     *
     * \param movement The movement vector, which is difference between
     *                 the start and the end of the movement
     * \param board The board with the characters
     * \return True if the character is able to move by this vector
     */
    bool move(const gf::Vector2i& movement, const Gameboard_t& board)
    {
        bool success{canMove(movement, board)};

        if (success) {
            m_pos += movement;
        }

        return success;
    }

    /**
     * Use the character's capacity
     *
     * \param target The target vector, which is difference between
     *                 the start and the end
     * \return True if the character is able to use its capacity by this vector
     */
    bool useCapacity(gf::Vector2i& target, Gameboard_t& board);

    /**
     * Tell if this character can move along a given vector
     *
     * \param movement The movement vector, which is difference between
     *                 the start and the end of the movement
     * \param board An array with all the characters
     * \param usedForNotPossibleDisplay Used for display purpose only. False by default. If true, does not consider view and if there is  character on the case
     * \return True if the character is able to move by this vector
     */
    bool canMove(const gf::Vector2i& movement, const Gameboard_t& board, bool usedForNotPossibleDisplay = false) const;

    /**
     * Tell if this character can use its capacity along a given vector
     *
     * \param target The target vector, which is difference between
     *                 the start and the end
     * \param board An array with all the characters
     * \param usedForNotPossibleDisplay Used for display purpose only. False by default. If true, does not consider view and if there is  character on the case
     * \return True if the character is able to use its capacity by this vector
     */
    bool canUseCapacity(const gf::Vector2i& target, const Gameboard_t& board, bool usedForNotPossibleDisplay = false) const;

    /**
     * Give all the actions the character can do
     * \param board The board
     * \return A vector with all the possible actions
     */
    std::vector<Action> getPossibleActions(const Gameboard_t& board);

private:
    /**
     * Give the maximum amount of HP according to the type of character
     *
     * \param type The type of character
     * \return The maximum number of of HP for this type
     */
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

        return -1; // to suppress the "no-return" warning
    }

    /**
     * Give the damage dealt according to the type of character
     *
     * \param type The type of character
     * \return The amount of damage this type of character deal
     */
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

        return -1; // to suppress the "no-return" warning
    }

    PlayerTeam m_team; ///< The team this character belongs to
    CharacterType m_type; ///< This character's type

    gf::Vector2i m_pos; ///< This character's current position

    int m_hp; ///< This character's current HP
};

#endif // CHARACTER_H
