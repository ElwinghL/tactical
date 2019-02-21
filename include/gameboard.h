/**
 * A file containing the game's board class
 * \author Fabien Matusalem
 * \date 28/01/19
 */

#ifndef CTHULHUVSSATAN_GAMEBOARD_H
#define CTHULHUVSSATAN_GAMEBOARD_H

#include "character.h"
#include "goal.h"

#include <gf/Array2D.h>

#include <boost/optional.hpp>
#include <functional>
#include <queue>
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
    constexpr static int goalsPerTeam = 2;
    constexpr static int charactersPerTeam = 6;

    explicit Gameboard();

    /**
     * Get a set of every possible movement for the character
     * \param usedForNotPossibleDisplay Used for display purpose only. If true, does not consider view and if there is character on the case
     * \return The set of possible relative vector movements
     */
    std::set<gf::Vector2i, PositionComp> getAllPossibleMoves(const gf::Vector2i& origin,
                                                             bool usedForNotPossibleDisplay = false) const
    {
        return getAllPossibleActionsOfAType(&Gameboard::canMove, origin, origin, usedForNotPossibleDisplay);
    }

    /**
     * Get a set of every possible attack for the character
     * \param usedForNotPossibleDisplay Used for display purpose only. If true, does not consider view and if there is character on the case
     * \return The set of possible relative vector attack
     */
    std::set<gf::Vector2i, PositionComp> getAllPossibleAttacks(const gf::Vector2i& origin,
                                                               bool usedForNotPossibleDisplay = false) const
    {
        return getAllPossibleActionsOfAType(&Gameboard::canAttack, origin, origin, usedForNotPossibleDisplay);
    }

    /**
     * Get a set of every possible capacity for the character
     * \param usedForNotPossibleDisplay Used for display purpose only. If true, does not consider view and if there is character on the case
     * \return The set of possible relative vector capacity
     */
    std::set<gf::Vector2i, PositionComp> getAllPossibleCapacities(const gf::Vector2i& origin,
                                                                  bool usedForNotPossibleDisplay = false) const
    {
        return getAllPossibleActionsOfAType(&Gameboard::canUseCapacity, origin, origin, usedForNotPossibleDisplay);
    }

    /**
     * Give all the actions a character can do
     * \param origin The position of the character to get its actions
     * \return A vector with all the possible actions
     */
    std::vector<Action> getPossibleActions(const gf::Vector2i& origin) const;

    /**
     * Attack another character
     *
     * The amount of damage dealt to the other character
     * depends of the type of this one
     *
     * \param origin The position of the attacking character
     * \param dest The position of the character to attack
     * \return True if the attack succeeds
     */
    bool attack(const gf::Vector2i& origin, const gf::Vector2i& dest);

    /**
     * Tell if the character can attack another one
     *
     * \param origin The position where the character will be while attacking
     * \param dest The position of the possibly attacked character
     * \param executor The position of the character trying to attack
     * \return Unable if the character never reach the dest position
     *         Unavailable the attack can't reach an enemy
     *         Able otherwise
     */
    Ability canAttack(const gf::Vector2i& origin, const gf::Vector2i& dest, const gf::Vector2i& executor) const;

    Ability canAttack(const gf::Vector2i& origin, const gf::Vector2i& dest) const
    {
        return canAttack(origin, dest, origin);
    }


    bool isLocked(gf::Vector2i pos) const;

    /**
     * Move this character
     *
     * \param origin The position of the character
     * \param dest The position where the character tries to move
     * \return True if the character has moved
     */
    bool move(const gf::Vector2i& origin, const gf::Vector2i& dest);

    /**
     * Tell if this character can move from a position to another
     *
     * \param origin The position of the character
     * \param dest The position where the character tries to move
     * \return Able if the character can move to the dest position
     *         Unavailable if the way to the dest position is blocked
     *         Unable otherwise
     */
    Ability canMove(const gf::Vector2i& origin, const gf::Vector2i& dest) const
    {
        return canMove(origin, dest, origin);
    }

    /**
     * Use the character's capacity
     *
     * \param origin The position of the character using its capacity
     * \param dest The position of target of the capacity
     * \return True if the capacity succeeds
     */
    bool useCapacity(const gf::Vector2i& origin, const gf::Vector2i& dest);

    /**
     * Tell if this character can use its capacity along a given vector
     *
     * \param origin The position where the character will be while using its capacity
     * \param dest The position of target of the capacity
     * \param executor The position of the character trying to use its capacity
     * \return Unable if the character never reach the dest position
     *         Unavailable the capacity is blocked or can't reach its target
     *         Able otherwise
     */
    Ability canUseCapacity(const gf::Vector2i& origin, const gf::Vector2i& dest, const gf::Vector2i& executor) const;

    Ability canUseCapacity(const gf::Vector2i& origin, const gf::Vector2i& dest) const
    {
        return canUseCapacity(origin, dest, origin);
    }

    bool capacityWillHurt(const gf::Vector2i& origin, const gf::Vector2i& dest) const
    {
        int ejectionDistance = 2;
        return m_array(origin)->getType() == CharacterType::Support && canUseCapacity(origin, dest) &&
               !isTargetReachable(dest, dest + ejectionDistance * gf::sign(dest - origin));
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
        return gf::Vector2i{12, 6};
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
                    case CharacterType::Tank:
                        std::cout << ((getTeamFor(pos) == PlayerTeam::Cthulhu) ? "T" : "t");
                        break;

                    case CharacterType::Scout:
                        std::cout << ((getTeamFor(pos) == PlayerTeam::Cthulhu) ? "E" : "e");
                        break;

                    case CharacterType::Support:
                        std::cout << ((getTeamFor(pos) == PlayerTeam::Cthulhu) ? "S" : "s");
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

    template<typename UnaryGoalFunc>
    void doWithGoals(UnaryGoalFunc f) const
    {
        for (auto& goal : m_goals) {
            f(goal);
        }
    }

    bool isGoal(const gf::Vector2i& pos, PlayerTeam team) const
    {
        for (auto& goal : m_goals) {
            if (goal.getPosition() == pos && goal.getTeam() == team) {
                return true;
            }
        }

        return false;
    }

    int getNbOfActivatedGoals(PlayerTeam team) const
    {
        int result = 0;
        doWithGoals([&result, &team](const Goal& goal) {
            if (goal.getTeam() == team && goal.isActivated()) {
                ++result;
            }
        });

        return result;
    }

    std::vector<gf::Vector2i> getTeamPositions(PlayerTeam team) const
    {
        std::vector<gf::Vector2i> results{};
        forEach([this, &results, &team](auto pos) {
            if (m_array(pos) && m_array(pos)->getTeam() == team) {
                results.push_back(std::move(pos));
            }
        });
        return results;
    }

    template<typename UnaryPositionFunc>
    constexpr void forEach(UnaryPositionFunc f) const
    {
        forEachPosition(getSize(), f);
    }

    bool hasWon(PlayerTeam team) const
    {
        return getNbOfActivatedGoals(team) == goalsPerTeam || getTeamPositions(getEnemyTeam(team)).empty();
    }

    template<typename BinaryFunc>
    void setMoveCallback(BinaryFunc f)
    {
        m_moveCallback = f;
    }

    template<typename BinaryFunc>
    void setHPChangeCallback(BinaryFunc f)
    {
        m_hpChangeCallback = f;
    }

    void callActionsCallbacks()
    {
        while (!m_lastActions.empty()) {
            (m_lastActions.front())();
            m_lastActions.pop();
        }
    }

    std::vector<int> getGoalsDistance(const gf::Vector2i pos) const
    {
        std::vector<int> ret;
        for (auto goal : m_goals) {
            auto gPos = goal.getPosition();
            if (!goal.isActivated()) {
                ret.push_back((gPos.x - pos.x) * (gPos.x - pos.x) + (gPos.y - pos.y) * (gPos.y - pos.y));
            }
        }
        return ret;
    }

private:
    void tryGoalActivation(PlayerTeam team, const gf::Vector2i& position)
    {
        for (auto& goal : m_goals) {
            if (goal.getPosition() == position && goal.getTeam() == team) {
                goal.activate();
            }
        }
    }

    void swapPositions(const gf::Vector2i& origin, const gf::Vector2i& dest)
    {
        assert(isOccupied(origin));
        assert(origin == dest || isEmpty(dest));

        boost::swap(m_array(origin), m_array(dest));
        tryGoalActivation(m_array(dest)->getTeam(), dest);
    }

    void swapOccupiedPositions(const gf::Vector2i& origin, const gf::Vector2i& dest)
    {
        assert(isOccupied(origin));
        assert(isOccupied(dest));

        boost::swap(m_array(origin), m_array(dest));
        tryGoalActivation(m_array(dest)->getTeam(), dest);
    }

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
            Ability (Gameboard::*canDoSomething)(const gf::Vector2i&, const gf::Vector2i&, const gf::Vector2i&) const,
            const gf::Vector2i& origin,
            const gf::Vector2i& executor,
            bool usedForNotPossibleDisplay) const;

    gf::Vector2i getLastReachablePos(const gf::Vector2i& origin, const gf::Vector2i& dest, bool excludeDest = false) const;

    bool isTargetReachable(const gf::Vector2i& origin, const gf::Vector2i& dest, bool excludeDest = false) const
    {
        gf::Vector2i lastReachablePos = getLastReachablePos(origin, dest, excludeDest);

        assert(gf::cross(dest - origin, lastReachablePos - origin) == 0);
        return gf::dot(dest - origin, lastReachablePos - dest) >= 0;
    }

    /**
     * Remove a character if its HP have fallen to 0
     * \param target the position of the character to test
     */
    void removeIfDead(const gf::Vector2i& target)
    {
        if (isOccupied(target) && m_array(target)->isDead()) {
            m_array(target) = boost::none;
        }
    }

    void pushLastMove(const gf::Vector2i& origin, const gf::Vector2i& dest)
    {
        m_lastActions.emplace(std::bind(m_moveCallback, origin, dest));
    }

    void pushLastHPChange(const gf::Vector2i& pos, int hp)
    {
        m_lastActions.emplace(std::bind(m_hpChangeCallback, pos, hp));
    }

    gf::Array2D<boost::optional<Character>> m_array;
    std::array<Goal, 2 * goalsPerTeam> m_goals;
    PlayerTeam m_playingTeam{PlayerTeam::Cthulhu};

    std::function<void(const gf::Vector2i&, const gf::Vector2i&)> m_moveCallback = [](auto, auto) {};
    std::function<void(const gf::Vector2i&, int)> m_hpChangeCallback = [](auto, auto) {};

    std::queue<std::function<void()>> m_lastActions{};
};


#endif //CTHULHUVSSATAN_GAMEBOARD_H
