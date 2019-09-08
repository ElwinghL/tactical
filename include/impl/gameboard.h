#ifndef IMPL_GAMEBOARD_H
#define IMPL_GAMEBOARD_H

#include <algorithm>

#include <cassert>

constexpr Ability::Ability(Value value) :
    m_value{value}
{
    //nothing
}

constexpr Ability::operator bool() const
{
    return m_value == Able;
}

constexpr bool Ability::operator!() const
{
    return m_value != Able;
}

constexpr Ability::operator Value() const
{
    return m_value;
}

[[nodiscard]] inline std::set<gf::Vector2i, PositionComp>
Gameboard::getAllPossibleMoves(const gf::Vector2i& origin, bool usedForNotPossibleDisplay) const
{
    return getAllPossibleActionsOfAType(&Gameboard::canMove, origin, origin, usedForNotPossibleDisplay);
}

[[nodiscard]] inline std::set<gf::Vector2i, PositionComp>
Gameboard::getAllPossibleAttacks(const gf::Vector2i& origin, bool usedForNotPossibleDisplay) const
{
    return getAllPossibleActionsOfAType(&Gameboard::canAttack, origin, origin, usedForNotPossibleDisplay);
}

[[nodiscard]] inline std::set<gf::Vector2i, PositionComp>
Gameboard::getAllPossibleCapacities(const gf::Vector2i& origin, bool usedForNotPossibleDisplay) const
{
    return getAllPossibleActionsOfAType(&Gameboard::canUseCapacity, origin, origin, usedForNotPossibleDisplay);
}

[[nodiscard]] inline Ability Gameboard::canAttack(const gf::Vector2i& origin, const gf::Vector2i& dest) const
{
    return canAttack(origin, dest, origin);
}

[[nodiscard]] inline Ability Gameboard::canMove(const gf::Vector2i& origin, const gf::Vector2i& dest) const
{
    return canMove(origin, dest, origin);
}

[[nodiscard]] inline Ability Gameboard::canUseCapacity(const gf::Vector2i& origin, const gf::Vector2i& dest) const
{
    return canUseCapacity(origin, dest, origin);
}

[[nodiscard]] inline bool Gameboard::capacityWillHurt(const gf::Vector2i& origin, const gf::Vector2i& dest) const
{
    assert(m_array.isValid(origin));
    constexpr int ejectionDistance = 2;
    return m_array(origin)->getType() == CharacterType::Support && canUseCapacity(origin, dest) &&
           !isTargetReachable(dest, dest + ejectionDistance * gf::sign(dest - origin));
}

[[nodiscard]] inline bool Gameboard::isEmpty(const gf::Vector2i& tile) const
{
    return m_array.isValid(tile) && m_array(tile) == std::nullopt;
}

[[nodiscard]] inline bool Gameboard::isOccupied(const gf::Vector2i& tile) const
{
    return m_array.isValid(tile) && m_array(tile) != std::nullopt;
}

[[nodiscard]] inline Character Gameboard::getCharacter(const gf::Vector2i& tile) const
{
    assert(isOccupied(tile));
    return *m_array(tile);
}

[[nodiscard]] constexpr gf::Vector2i Gameboard::getSize() const
{
    return gf::Vector2i{12, 6};
}

[[nodiscard]] inline PlayerTeam Gameboard::getTeamFor(const gf::Vector2i& tile) const
{
    assert(isOccupied(tile));
    return m_array(tile)->getTeam();
}

[[nodiscard]] inline CharacterType Gameboard::getTypeFor(const gf::Vector2i& tile) const
{
    assert(isOccupied(tile));
    return m_array(tile)->getType();
}

[[nodiscard]] constexpr PlayerTeam Gameboard::getPlayingTeam() const
{
    return m_playingTeam;
}

constexpr void Gameboard::switchTurn()
{
    m_playingTeam = getEnemyTeam(m_playingTeam);
}

template<typename UnaryGoalFunc>
constexpr void Gameboard::doWithGoals(UnaryGoalFunc f) const
{
    for (auto& goal : m_goals) {
        f(goal);
    }
}

[[nodiscard]] inline bool Gameboard::isGoal(const gf::Vector2i& pos, PlayerTeam team) const
{
    return std::any_of(m_goals.begin(), m_goals.end(), [&pos, &team] (const auto& goal) {
        return goal.getPosition() == pos && goal.getTeam() == team;
    });
}

template<typename UnaryPositionFunc>
constexpr void Gameboard::forEach(UnaryPositionFunc f) const
{
    gf::Vector2i size = getSize();
    for (int y = 0; y < size.height; ++y) {
        for (int x = size.width - 1; x >= 0; --x) {
            f(gf::Vector2i{x, y});
        }
    }
}

[[nodiscard]] inline bool Gameboard::hasWon(PlayerTeam team) const
{
    return getNbOfActivatedGoals(team) == goalsPerTeam || getTeamPositions(getEnemyTeam(team)).empty();
}

template<typename BinaryFunc>
inline void Gameboard::setMoveCallback(BinaryFunc f)
{
    m_moveCallback = f;
}

template<typename BinaryFunc>
inline void Gameboard::setHPChangeCallback(BinaryFunc f)
{
    m_hpChangeCallback = f;
}

[[nodiscard]] inline bool Gameboard::isCallbackNeeded() const
{
    return !m_lastActions.empty();
}

inline void Gameboard::doFirstCallback()
{
    (m_lastActions.front())();
    m_lastActions.pop();
}

inline bool Gameboard::operator==(const Gameboard& other) const
{
    return std::tie(m_array, m_goals, m_playingTeam) == std::tie(other.m_array, other.m_goals, other.m_playingTeam);
}

inline void Gameboard::swapPositions(const gf::Vector2i& origin, const gf::Vector2i& dest)
{
    assert(isOccupied(origin));
    assert(origin == dest || isEmpty(dest));

    std::swap(m_array(origin), m_array(dest));
    tryGoalActivation(m_array(dest)->getTeam(), dest);
}

inline void Gameboard::swapOccupiedPositions(const gf::Vector2i& origin, const gf::Vector2i& dest)
{
    assert(isOccupied(origin));
    assert(isOccupied(dest));

    std::swap(m_array(origin), m_array(dest));
    tryGoalActivation(m_array(dest)->getTeam(), dest);
}

[[nodiscard]] inline std::set<gf::Vector2i, PositionComp>
    Gameboard::getAllPossibleAttacks(const gf::Vector2i& origin, const gf::Vector2i& executor) const
{
    return getAllPossibleActionsOfAType(&Gameboard::canAttack, origin, executor, false);
}

[[nodiscard]] inline std::set<gf::Vector2i, PositionComp>
    Gameboard::getAllPossibleCapacities(const gf::Vector2i& origin, const gf::Vector2i& executor) const
{
    return getAllPossibleActionsOfAType(&Gameboard::canUseCapacity, origin, executor, false);
}

[[nodiscard]] inline bool Gameboard::isTargetReachable(const gf::Vector2i& origin, const gf::Vector2i& dest, bool excludeDest) const
{
    gf::Vector2i lastReachablePos = getLastReachablePos(origin, dest, excludeDest);

    assert(gf::cross(dest - origin, lastReachablePos - origin) == 0);
    return gf::dot(dest - origin, lastReachablePos - dest) >= 0;
}

inline void Gameboard::removeIfDead(const gf::Vector2i& target)
{
    if (isOccupied(target) && m_array(target)->isDead()) {
        m_array(target) = std::nullopt;
    }
}

inline void Gameboard::pushLastMove(const gf::Vector2i& origin, const gf::Vector2i& dest)
{
    if (origin != dest) {
        m_lastActions.emplace(std::bind(m_moveCallback, origin, dest));
    }
}

inline void Gameboard::pushLastHPChange(const gf::Vector2i& pos, int hp)
{
    m_lastActions.emplace(std::bind(m_hpChangeCallback, pos, hp));
}

#endif //IMPL_GAMEBOARD_H
