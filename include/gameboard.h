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

#include <functional>
#include <optional>
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

    constexpr Ability(Value value);

    constexpr operator bool() const;
    constexpr bool operator!() const;

    constexpr operator Value() const;

private:
    Value m_value;
};

class Gameboard {
public:
    using BitsType = std::pair<std::uint64_t, std::uint64_t>;

    constexpr static int goalsPerTeam = 2;
    constexpr static int charactersPerTeam = 6;

    explicit Gameboard();

    /**
     * Get a set of every possible movement for the character
     * \param usedForNotPossibleDisplay Used for display purpose only. If true, does not consider view and if there is character on the case
     * \return The set of possible relative vector movements
     */
    [[nodiscard]] inline std::set<gf::Vector2i, PositionComp>
        getAllPossibleMoves(const gf::Vector2i& origin, bool usedForNotPossibleDisplay = false) const;

    /**
     * Get a set of every possible attack for the character
     * \param usedForNotPossibleDisplay Used for display purpose only. If true, does not consider view and if there is character on the case
     * \return The set of possible relative vector attack
     */
    [[nodiscard]] inline std::set<gf::Vector2i, PositionComp>
        getAllPossibleAttacks(const gf::Vector2i& origin, bool usedForNotPossibleDisplay = false) const;

    /**
     * Get a set of every possible capacity for the character
     * \param usedForNotPossibleDisplay Used for display purpose only. If true, does not consider view and if there is character on the case
     * \return The set of possible relative vector capacity
     */
    [[nodiscard]] inline std::set<gf::Vector2i, PositionComp>
        getAllPossibleCapacities(const gf::Vector2i& origin, bool usedForNotPossibleDisplay = false) const;

    /**
     * Give all the actions a character can do
     * \param origin The position of the character to get its actions
     * \return A vector with all the possible actions
     */
    [[nodiscard]] std::vector<Action> getPossibleActions(const gf::Vector2i& origin) const;
    [[nodiscard]] std::vector<Action> getPossibleActions() const;

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
    [[nodiscard]] Ability canAttack(const gf::Vector2i& origin, const gf::Vector2i& dest, const gf::Vector2i& executor) const;
    [[nodiscard]] inline Ability canAttack(const gf::Vector2i& origin, const gf::Vector2i& dest) const;

    [[nodiscard]] bool isLocked(const gf::Vector2i& pos) const;

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
    [[nodiscard]] inline Ability canMove(const gf::Vector2i& origin, const gf::Vector2i& dest) const;

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
    [[nodiscard]] Ability canUseCapacity(const gf::Vector2i& origin, const gf::Vector2i& dest, const gf::Vector2i& executor) const;
    [[nodiscard]] inline Ability canUseCapacity(const gf::Vector2i& origin, const gf::Vector2i& dest) const;

    [[nodiscard]] inline bool capacityWillHurt(const gf::Vector2i& origin, const gf::Vector2i& dest) const;

    [[nodiscard]] inline bool isEmpty(const gf::Vector2i& tile) const;
    [[nodiscard]] inline bool isOccupied(const gf::Vector2i& tile) const;

    [[nodiscard]] inline Character getCharacter(const gf::Vector2i& tile) const;

    [[nodiscard]] constexpr gf::Vector2i getSize() const;

    [[nodiscard]] inline PlayerTeam getTeamFor(const gf::Vector2i& tile) const;
    [[nodiscard]] inline CharacterType getTypeFor(const gf::Vector2i& tile) const;

    void display() const;

    [[nodiscard]] constexpr PlayerTeam getPlayingTeam() const;

    /**
     * Switch turn
     */
    constexpr void switchTurn();

    template<typename UnaryGoalFunc>
    constexpr void doWithGoals(UnaryGoalFunc f) const;

    [[nodiscard]] inline bool isGoal(const gf::Vector2i& pos, PlayerTeam team) const;

    [[nodiscard]] int getNbOfActivatedGoals(PlayerTeam team) const;
    
    template<typename UnaryPositionFunc>
    constexpr void forEach(UnaryPositionFunc f) const;

    [[nodiscard]] std::vector<gf::Vector2i> getTeamPositions(PlayerTeam team) const;

    [[nodiscard]] inline bool hasWon(PlayerTeam team) const;

    template<typename BinaryFunc>
    inline void setMoveCallback(BinaryFunc f);

    template<typename BinaryFunc>
    inline void setHPChangeCallback(BinaryFunc f);

    [[nodiscard]] inline bool isCallbackNeeded() const;

    inline void doFirstCallback();

    [[nodiscard]] std::array<int, 2 * goalsPerTeam> getGoalsDistance(const gf::Vector2i& pos) const;

    inline bool operator==(const Gameboard& other) const;

    [[nodiscard]] BitsType computeBitRepresentation() const;

private:
    void tryGoalActivation(PlayerTeam team, const gf::Vector2i& position);

    inline void swapPositions(const gf::Vector2i& origin, const gf::Vector2i& dest);
    inline void swapOccupiedPositions(const gf::Vector2i& origin, const gf::Vector2i& dest);

    [[nodiscard]] Ability canMove(const gf::Vector2i& origin, const gf::Vector2i& dest, const gf::Vector2i& /*executor*/) const;

    [[nodiscard]] inline std::set<gf::Vector2i, PositionComp>
        getAllPossibleAttacks(const gf::Vector2i& origin, const gf::Vector2i& executor) const;
    [[nodiscard]] inline std::set<gf::Vector2i, PositionComp>
        getAllPossibleCapacities(const gf::Vector2i& origin, const gf::Vector2i& executor) const;

    [[nodiscard]] std::set<gf::Vector2i, PositionComp> getAllPossibleActionsOfAType(
            Ability (Gameboard::*canDoSomething)(const gf::Vector2i&, const gf::Vector2i&, const gf::Vector2i&) const,
            const gf::Vector2i& origin,
            const gf::Vector2i& executor,
            bool usedForNotPossibleDisplay) const;

    [[nodiscard]] gf::Vector2i getLastReachablePos(const gf::Vector2i& origin, const gf::Vector2i& dest, bool excludeDest = false) const;
    [[nodiscard]] inline bool isTargetReachable(const gf::Vector2i& origin, const gf::Vector2i& dest, bool excludeDest = false) const;

    /**
     * Remove a character if its HP have fallen to 0
     * \param target the position of the character to test
     */
    inline void removeIfDead(const gf::Vector2i& target);

    inline void pushLastMove(const gf::Vector2i& origin, const gf::Vector2i& dest);
    inline void pushLastHPChange(const gf::Vector2i& pos, int hp);

    gf::Array2D<std::optional<Character>> m_array;
    std::array<Goal, 2 * goalsPerTeam> m_goals;
    PlayerTeam m_playingTeam{PlayerTeam::Cthulhu};

    std::function<void(const gf::Vector2i&, const gf::Vector2i&)> m_moveCallback = [](auto, auto) {};
    std::function<void(const gf::Vector2i&, int)> m_hpChangeCallback = [](auto, auto) {};

    std::queue<std::function<void()>> m_lastActions{};
};

#include "impl/gameboard.h"

#endif //CTHULHUVSSATAN_GAMEBOARD_H
