//
// Created by fabien on 28/01/19.
//

#ifndef CTHULHUVSSATAN_GAMEBOARD_H
#define CTHULHUVSSATAN_GAMEBOARD_H

#include "character.h"

#include <gf/Array2D.h>

#include <boost/optional.hpp>

#include <set>
#include <vector>

class Action;

class Ability {
public:
    enum class Value {
        Unable,
        Unavailable,
        Able
    };

    static constexpr auto Unable = Value::Unable;
    static constexpr auto Unavailable = Value::Unavailable;
    static constexpr auto Able = Value::Able;

    constexpr Ability(Value value) :
        m_value{value}
    {
    }

    constexpr operator bool() const
    {
        return m_value == Able;
    }

    constexpr bool operator!() const
    {
        return m_value != Able;
    }

    constexpr operator Value() const
    {
        return m_value;
    }

private:
    Value m_value;
};

class Gameboard {
public:
    explicit Gameboard();

    /**
    * Get a set of every possible movement for the character
     * \param board The board containing pointers to characters
     * \param usedForNotPossibleDisplay Used for display purpose only. False by default. If true, does not consider view and if there is  character on the case
     * \return The set of possible relative vector movements
     */
    std::set<gf::Vector2i, PositionComp> getAllPossibleMoves(const gf::Vector2i& origin,
                                                             bool usedForNotPossibleDisplay = false) const
    {
        return getAllPossibleActionsOfAType(&Gameboard::canMove, origin, origin, usedForNotPossibleDisplay);
    }

    /**
     * Get a set of every possible attack for the character
     * \param board The board containing pointers to characters
     * \param usedForNotPossibleDisplay Used for display purpose only. False by default. If true, does not consider view and if there is  character on the case
     * \return The set of possible relative vector attack
     */
    std::set<gf::Vector2i, PositionComp> getAllPossibleAttacks(const gf::Vector2i& origin,
                                                               bool usedForNotPossibleDisplay = false) const
    {
        return getAllPossibleActionsOfAType(&Gameboard::canAttack, origin, origin, usedForNotPossibleDisplay);
    }

    /**
     * Get a set of every possible capacity for the character
     * \param board The board containing pointers to characters
     * \param usedForNotPossibleDisplay Used for display purpose only. False by default. If true, does not consider view and if there is  character on the case
     * \return The set of possible relative vector capacity
     */
    std::set<gf::Vector2i, PositionComp> getAllPossibleCapacities(const gf::Vector2i& origin,
                                                                  bool usedForNotPossibleDisplay = false) const
    {
        return getAllPossibleActionsOfAType(&Gameboard::canUseCapacity, origin, origin,
                                            usedForNotPossibleDisplay);
    }

    /**
     * Give all the actions the character can do
     * \param board The board
     * \return A vector with all the possible actions
     */
    std::vector<Action> getPossibleActions(const gf::Vector2i& origin) const;

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
    bool attack(const gf::Vector2i& origin, const gf::Vector2i& dest);

    /**
     * Tell if the character can attack another one
     *
     * \param other The other character this character is trying to attack
     * \param board The board of the game
     * \param usedForNotPossibleDisplay Used for display purpose only. False by default. If true, does not consider view and if there is  character on the case
     * \return True if the other character may be attacked by this character
     */
    Ability canAttack(const gf::Vector2i& origin, const gf::Vector2i& dest, const gf::Vector2i& executor) const;

    Ability canAttack(const gf::Vector2i& origin, const gf::Vector2i& dest) const
    {
        return canAttack(origin, dest, origin);
    }

    /**
     * Move this character
     *
     * \param dest The movement vector, which is difference between
     *                 the start and the end of the movement
     * \param board The board with the characters
     * \return True if the character is able to move by this vector
     */
    bool move(const gf::Vector2i& origin, const gf::Vector2i& dest);

    /**
     * Tell if this character can move along a given vector
     *
     * \param dest The movement vector, which is difference between
     *                 the start and the end of the movement
     * \param board An array with all the characters
     * \param usedForNotPossibleDisplay Used for display purpose only. False by default. If true, does not consider view and if there is  character on the case
     * \return True if the character is able to move by this vector
     */
    Ability canMove(const gf::Vector2i& origin, const gf::Vector2i& dest) const
    {
        return canMove(origin, dest, origin);
    }

    /**
     * Use the character's capacity
     *
     * \param dest The target vector, which is difference between
     *                 the start and the end
     * \return True if the character is able to use its capacity by this vector
     */
    bool useCapacity(const gf::Vector2i& origin, const gf::Vector2i& dest);

    /**
     * Tell if this character can use its capacity along a given vector
     *
     * \param dest The target vector, which is difference between
     *                 the start and the end
     * \param board An array with all the characters
     * \param usedForNotPossibleDisplay Used for display purpose only. False by default. If true, does not consider view and if there is  character on the case
     * \return True if the character is able to use its capacity by this vector
     */
    Ability canUseCapacity(const gf::Vector2i& origin, const gf::Vector2i& dest, const gf::Vector2i& executor) const;

    Ability canUseCapacity(const gf::Vector2i& origin, const gf::Vector2i& dest) const
    {
        return canUseCapacity(origin, dest, origin);
    }

    bool isEmpty(const gf::Vector2i& tile) const
    {
        return m_array.isValid(tile) && m_array(tile) == boost::none;
    }

    bool isOccupied(const gf::Vector2i& tile) const
    {
        return m_array.isValid(tile) && m_array(tile) != boost::none;
    }

    Character getCharacter(const gf::Vector2i& tile) const
    {
        assert(isOccupied(tile));
        return *m_array(tile);
    }

    gf::Vector2i getSize() const
    {
        return m_array.getSize();
    }

    PlayerTeam getTeamFor(const gf::Vector2i& tile) const
    {
        assert(isOccupied(tile));
        return m_array(tile)->getTeam();
    }

    CharacterType getTypeFor(const gf::Vector2i& tile) const
    {
        assert(isOccupied(tile));
        return m_array(tile)->getType();
    }

    void display() const
    {
        for (gf::Vector2i pos{0, 0}, size = m_array.getSize(); pos.y < size.height; ++pos.y) {
            for (pos.x = 0; pos.x < size.width; ++pos.x) {
                if (m_array(pos)) {
                    switch (getTypeFor(pos)) {
                    case CharacterType::Tank:std::cout << ((getTeamFor(pos) == PlayerTeam::Cthulhu) ? "T" : "t");
                        break;

                    case CharacterType::Scout:std::cout << ((getTeamFor(pos) == PlayerTeam::Cthulhu) ? "E" : "e");
                        break;

                    case CharacterType::Support:std::cout << ((getTeamFor(pos) == PlayerTeam::Cthulhu) ? "S" : "s");
                        break;
                    }
                } else {
                    std::cout << " ";
                }
                std::cout << " ";
            }
            std::cout << std::endl;
        }
    }

    PlayerTeam getPlayingTeam() const
    {
        return m_playingTeam;
    }

    /**
     * Switch turn
     */
    void switchTurn()
    {
        m_playingTeam = getEnemyTeam(m_playingTeam);
    }

private:
    Ability canMove(const gf::Vector2i& origin, const gf::Vector2i& dest, const gf::Vector2i& /*executor*/) const;

    std::set<gf::Vector2i, PositionComp> getAllPossibleAttacks(const gf::Vector2i& origin,
                                                               const gf::Vector2i& executor) const
    {
        return getAllPossibleActionsOfAType(&Gameboard::canAttack, origin, executor, false);
    }


    std::set<gf::Vector2i, PositionComp> getAllPossibleCapacities(const gf::Vector2i& origin,
                                                                  const gf::Vector2i& executor) const
    {
        return getAllPossibleActionsOfAType(&Gameboard::canUseCapacity, origin, executor, false);
    }

    std::set<gf::Vector2i, PositionComp> getAllPossibleActionsOfAType(
        Ability (Gameboard::* canDoSomething)(const gf::Vector2i&, const gf::Vector2i&, const gf::Vector2i&) const,
        const gf::Vector2i& origin,
        const gf::Vector2i& executor, bool usedForNotPossibleDisplay) const;

    gf::Vector2i getLastReachablePos(const gf::Vector2i& origin, const gf::Vector2i& dest,
                                     bool excludeDest = false) const;

    bool isTargetReachable(const gf::Vector2i& origin, const gf::Vector2i& dest, bool excludeDest = false) const
    {
        gf::Vector2i lastReachablePos = getLastReachablePos(origin, dest, excludeDest);

        assert(gf::cross(dest - origin, lastReachablePos - origin) == 0);
        return gf::dot(dest - origin, lastReachablePos - dest) >= 0;
    }

    /**
     * Remove a character everywhere its informations are saved if it is dead
     * \param target the position of the character
     */
    void removeIfDead(const gf::Vector2i& target)
    {
        if (isOccupied(target) && m_array(target)->isDead()) {
            m_array(target) = boost::none;
        }
    }

    gf::Array2D<boost::optional<Character>> m_array;
    PlayerTeam m_playingTeam{PlayerTeam::Cthulhu}; ///< Can the player play?
};


#endif //CTHULHUVSSATAN_GAMEBOARD_H
