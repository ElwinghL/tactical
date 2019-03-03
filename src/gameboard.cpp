#include "gameboard.h"

#include "action.h"

#include <bitset>

Gameboard::Gameboard() :
    m_array{getSize(), boost::none},
    m_goals{
            Goal{PlayerTeam::Cthulhu, {10, 1}},
            Goal{PlayerTeam::Cthulhu, {10, 4}},
            Goal{PlayerTeam::Satan, {1, 1}},
            Goal{PlayerTeam::Satan, {1, 4}},
    }
{
    auto initPlayerCharacters{[this](int column, PlayerTeam team) {
        gf::Vector2i pos{column, 0};

        auto addCharacterInColumn{[this, &pos, &team](CharacterType type) {
            assert(m_array.isValid(pos));
            m_array(pos) = Character{team, type};
            ++pos.y;
        }};

        addCharacterInColumn(CharacterType::Scout);
        addCharacterInColumn(CharacterType::Support);
        addCharacterInColumn(CharacterType::Tank);
        addCharacterInColumn(CharacterType::Tank);
        addCharacterInColumn(CharacterType::Support);
        addCharacterInColumn(CharacterType::Scout);
    }};

    initPlayerCharacters(2, PlayerTeam::Cthulhu);
    initPlayerCharacters(9, PlayerTeam::Satan);
}

std::vector<Action> Gameboard::getPossibleActions(const gf::Vector2i& origin) const
{
    std::vector<Action> res = std::vector<Action>{};
    std::set<gf::Vector2i, PositionComp> possibleMovements = getAllPossibleMoves(origin);
    for (auto possibleMovement : possibleMovements) {
        res.emplace_back(origin, possibleMovement);

        std::set<gf::Vector2i, PositionComp> possibleCapacities = getAllPossibleCapacities(possibleMovement, origin);
        for (auto possibleCapacity : possibleCapacities) {
            res.emplace_back(ActionType::Capacity, origin, possibleMovement, possibleCapacity);
        }

        std::set<gf::Vector2i, PositionComp> possibleAttacks = getAllPossibleAttacks(possibleMovement, origin);
        for (auto possibleAttack : possibleAttacks) {
            res.emplace_back(ActionType::Attack, origin, possibleMovement, possibleAttack);
        }
    }

    return res;
}

bool Gameboard::attack(const gf::Vector2i& origin, const gf::Vector2i& dest)
{
    bool success = canAttack(origin, dest);

    if (success) {
        assert(isOccupied(origin));
        assert(isOccupied(dest));
        m_array(origin)->attack(*m_array(dest));
        pushLastHPChange(dest, m_array(dest)->getHP());
        removeIfDead(dest);
    }

    return success;
}

Ability Gameboard::canAttack(const gf::Vector2i& origin, const gf::Vector2i& dest, const gf::Vector2i& executor) const
{
    assert(isOccupied(executor));

    if (!m_array.isValid(dest)) {
        return Ability::Unable;
    }

    Ability result = Ability::Unable;
    gf::Vector2i relative = dest - origin;
    switch (getTypeFor(executor)) {
    case CharacterType::Scout: {
        if (gf::manhattanDistance(origin, dest) <= 1) {
            gf::Vector2i direction = gf::sign(relative);
            assert(m_array.isValid(origin + direction));
            result = (direction == relative || !m_array(origin + direction)) ? Ability::Able :
                     Ability::Unavailable; // Can't attack through another character
        }
    } break;

    case CharacterType::Support: {
        constexpr int range = 3;

        if (isOrthogonal(origin, dest) && gf::chebyshevLength(relative) <= range) {
            result = isTargetReachable(origin, dest, true) ? Ability::Able : Ability::Unavailable;
        }
    } break;

    case CharacterType::Tank:
        if (gf::chebyshevDistance(origin, dest) == 1) {
            result = Ability::Able;
        }
    }

    if (result) {
        auto target = m_array(dest);
        return (target && target->getTeam() != getTeamFor(executor)) ? Ability::Able : Ability::Unavailable;
    }

    return result;
}

bool Gameboard::move(const gf::Vector2i& origin, const gf::Vector2i& dest)
{
    bool success{canMove(origin, dest)};

    if (success) {
        swapPositions(origin, dest);
        pushLastMove(origin, dest);
    }

    return success;
}

Ability Gameboard::canMove(const gf::Vector2i& origin, const gf::Vector2i& dest, const gf::Vector2i& /*executor*/) const
{
    assert(isOccupied(origin));

    if (!m_array.isValid(dest)) {
        return Ability::Unable;
    }

    if (origin == dest) { // Don't move
        return Ability::Able;
    }

    gf::Vector2i relative = dest - origin;
    Ability result = Ability::Unable;

    switch (getTypeFor(origin)) {
    case CharacterType::Scout: {
        constexpr int range = 2;

        if ((isOrthogonal(origin, dest) || isDiagonal(origin, dest)) && gf::chebyshevLength(relative) <= range) {
            result = isTargetReachable(origin, dest) ? Ability::Able : Ability::Unavailable;
        }
    } break;

    case CharacterType::Tank: {
        constexpr int sideRange = 2;

        if (gf::chebyshevDistance(origin, dest) == 1 || (relative.x == 0 && std::abs(relative.y) == sideRange)) {
            result = isTargetReachable(origin, dest) ? Ability::Able : Ability::Unavailable;
        }
    } break;

    case CharacterType::Support: {
        if (gf::manhattanDistance(origin, dest) == 3 && relative.x != 0 && relative.y != 0) {
            result = Ability::Able;
        }
    } break;
    }

    if (result) {
        if (m_array(dest)) { // If there is already a character at this location
            result = Ability::Unavailable;
        } else { // Check for nearby enemy Tank
            if (isLocked(origin)) {
                result = Ability::Unavailable;
            }
        }
    }

    return result;
}

bool Gameboard::isLocked(const gf::Vector2i& pos) const
{
    if (!m_array.isValid(pos)) {
        return false;
    }
    constexpr gf::Orientation cardinals[] = {gf::Orientation::North, gf::Orientation::East, gf::Orientation::South,
                                             gf::Orientation::West};
    for (auto card : cardinals) {
        gf::Vector2i sq2Check = pos + gf::displacement(card);
        if (m_array.isValid(sq2Check)) {
            auto character2Check = m_array(sq2Check);
            if (character2Check && character2Check->getTeam() == getEnemyTeam(getTeamFor(pos)) &&
                character2Check->getType() == CharacterType::Tank) {
                return true;
            }
        }
    }
    return false;
}

bool Gameboard::useCapacity(const gf::Vector2i& origin, const gf::Vector2i& dest)
{
    if (!canUseCapacity(origin, dest)) {
        return false;
    }

    assert(m_array.isValid(origin));
    assert(m_array.isValid(dest));

    switch (getTypeFor(origin)) {
    case CharacterType::Scout: {
        pushLastMove(origin, dest);
        swapOccupiedPositions(origin, dest);
    } break;

    case CharacterType::Tank: {
        gf::Vector2i newPos = origin + gf::sign(dest - origin);
        pushLastMove(dest, newPos);
        swapPositions(dest, newPos);
    } break;

    case CharacterType::Support: {
        constexpr int ejectionDistance = 2;
        constexpr int ejectionDamage = 4;

        assert(m_array(dest));

        gf::Vector2i ejectedPos = dest + ejectionDistance * gf::sign(dest - origin);

        if (!isTargetReachable(dest, ejectedPos)) {
            m_array(dest)->damage(ejectionDamage);
            ejectedPos = getLastReachablePos(dest, ejectedPos);
            pushLastMove(dest, ejectedPos);
            pushLastHPChange(ejectedPos, m_array(dest)->getHP());
        } else {
            pushLastMove(dest, ejectedPos);
        }

        swapPositions(dest, ejectedPos);
        removeIfDead(ejectedPos);
    } break;
    }
    return true;
}

Ability Gameboard::canUseCapacity(const gf::Vector2i& origin, const gf::Vector2i& dest, const gf::Vector2i& executor) const
{
    assert(isOccupied(executor));

    if (!m_array.isValid(dest)) {
        return Ability::Unable;
    }

    int manhattanDist = gf::manhattanDistance(origin, dest);
    switch (getTypeFor(executor)) {
    case CharacterType::Scout: {
        if (isDiagonal(origin, dest) && (manhattanDist == 2 || manhattanDist == 4)) {
            return m_array(dest) ? Ability::Able : Ability::Unavailable;
        }
    } break;

    case CharacterType::Support: {
        if (isOrthogonal(origin, dest) && manhattanDist == 2) {
            return m_array(dest) ? Ability::Able : Ability::Unavailable;
        }
    } break;

    case CharacterType::Tank: {
        if (isOrthogonal(origin, dest) && (manhattanDist == 2 || manhattanDist == 3)) {
            return (m_array(dest) && isTargetReachable(origin, dest, true)) ? Ability::Able : Ability::Unavailable;
        }
    } break;
    }

    return Ability::Unable;
}

gf::Vector2i Gameboard::getLastReachablePos(const gf::Vector2i& origin,
                                            const gf::Vector2i& dest,
                                            bool excludeDest) const
{
    if ((!isOrthogonal(origin, dest) && !isDiagonal(origin, dest)) || origin == dest) {
        return origin;
    }

    const gf::Vector2i direction = gf::sign(dest - origin);

    gf::Vector2i result = origin;
    gf::Vector2i sq2Check = result + direction;

    while (isEmpty(sq2Check)) {
        result = sq2Check;
        sq2Check += direction;
    }

    if (excludeDest && isOccupied(sq2Check)) {
        return sq2Check;
    }

    return result;
}

std::set<gf::Vector2i, PositionComp> Gameboard::getAllPossibleActionsOfAType(
        Ability (Gameboard::*canDoSomething)(const gf::Vector2i&, const gf::Vector2i&, const gf::Vector2i&) const,
        const gf::Vector2i& origin,
        const gf::Vector2i& executor,
        bool usedForNotPossibleDisplay) const
{
    std::set<gf::Vector2i, PositionComp> res;
    forEach([this, &canDoSomething, &origin, &executor, &usedForNotPossibleDisplay, &res](auto pos) {
        Ability possibleAction = (this->*canDoSomething)(origin, pos, executor);
        if (usedForNotPossibleDisplay) {
            if (possibleAction != Ability::Unable) {
                res.insert(pos);
            }
        } else if (possibleAction == Ability::Able) {
            res.insert(pos);
        }
    });
    return res;
}

std::vector<Action> Gameboard::getPossibleActions() const
{
    std::vector<Action> results{};
    forEach([this, &results](auto pos) {
        if (isOccupied(pos) && getTeamFor(pos) == m_playingTeam) {
            auto characterActions = getPossibleActions(pos);
            results.insert(results.end(), characterActions.begin(), characterActions.end());
        }
    });

    return results;
}

void Gameboard::display() const
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
            } else if (isGoal(pos, PlayerTeam::Cthulhu) || isGoal(pos, PlayerTeam::Satan)) {
                for (auto& goal : m_goals) {
                    if (goal.getPosition() == pos) {
                        std::cout << (goal.isActivated() ? "X" : "O");
                    }
                }
            } else {
                std::cout << " ";
            }
            std::cout << " ";
        }
        std::cout << "\n";
    }

    std::cout << "Playing: " << ((m_playingTeam == PlayerTeam::Cthulhu) ? "Cthulhu" : "Satan") << std::endl;
}

Gameboard::HashType Gameboard::computeHash() const
{
    constexpr std::size_t xBitCount = 4;
    constexpr std::size_t yBitCount = 3;
    constexpr std::size_t hpBitCount = 3;

    constexpr std::size_t characterBitCount = xBitCount + yBitCount + hpBitCount;
    constexpr std::size_t resultBitCount = (characterBitCount * charactersPerTeam + goalsPerTeam) * 2 + 1;

    std::bitset<resultBitCount> bitResult{};

    std::bitset<charactersPerTeam> occupiedBits{};

    forEach([this, &bitResult, &occupiedBits](auto pos) {
        if (m_array(pos)) {
            auto& c = *m_array(pos);

            std::bitset<characterBitCount> characterState{};

            if (c.isDead()) {
                characterState.set();
            } else {
                characterState |= pos.x & ((1U << xBitCount) - 1);
                characterState |= (pos.y & ((1U << yBitCount) - 1)) << xBitCount;
                characterState |= ((c.getHP() - 1) & ((1U << hpBitCount) - 1)) << (xBitCount + yBitCount);
            }

            std::size_t bitPos = 0;
            switch (c.getType()) {
            case CharacterType::Tank: {
                // Nothing, tank at 0
            }
                break;

            case CharacterType::Support: {
                bitPos = 2;
            }
                break;

            case CharacterType::Scout: {
                bitPos = 4;
            }
                break;
            }

            if (c.getTeam() == PlayerTeam::Satan) {
                bitPos += 1;
            }

            if (occupiedBits.test(bitPos)) {
                bitPos += charactersPerTeam;
            } else {
                occupiedBits.set(bitPos);
            }

            bitPos *= characterBitCount;

            bitResult |= std::bitset<resultBitCount>{characterState.to_ullong()} << bitPos;
        }
    });

    for (std::size_t i = 0; i < m_goals.size(); ++i) {
        if (m_goals[i].isActivated()) {
            bitResult.set(i + characterBitCount * charactersPerTeam * 2);
        }
    }

    if (m_playingTeam == PlayerTeam::Satan) {
        bitResult.set(resultBitCount - 1);
    }

    HashType result{0, 0};
    result.second = static_cast<uint64_t>(
        (bitResult & std::bitset<resultBitCount>{std::numeric_limits<unsigned long long>::max()}).to_ullong() &
        std::numeric_limits<uint64_t>::max());
    bitResult >>= 64;
    result.first = static_cast<uint64_t>(bitResult.to_ullong() & std::numeric_limits<uint64_t>::max());

    return result;
}
