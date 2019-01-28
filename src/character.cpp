#include "character.h"

#include "action.h"

std::vector<Action> Character::getPossibleActions(const Gameboard_t& board)
{
    std::vector<Action> res = std::vector<Action>{};
    std::set<gf::Vector2i, PositionComp> possibleMovements = getAllPossibleMoves(board);
    for (auto possibleMovement : possibleMovements) {
        res.emplace_back(m_pos, possibleMovement);

        Gameboard_t thisBoard{board};
        auto character = thisBoard(m_pos);
        assert(character);

        character->move(thisBoard, m_pos, possibleMovement);

        std::set<gf::Vector2i, PositionComp> possibleCapacities = character->getAllPossibleCapacities(thisBoard);
        for (auto possibleCapacity : possibleCapacities) {
            res.emplace_back(ActionType::Capacity, m_pos, possibleMovement, possibleCapacity);
        }

        std::set<gf::Vector2i, PositionComp> possibleAttacks = character->getAllPossibleAttacks(thisBoard);
        for (auto possibleAttack : possibleAttacks) {
            res.emplace_back(ActionType::Attack, m_pos, possibleMovement, possibleAttack);
        }
    }

    return res;
}

bool Character::attack(Gameboard_t& board, const gf::Vector2i& origin, const gf::Vector2i& dest) const
{
    if (canAttack(board, origin, dest)) {
        board(dest)->damage(getDamageForType(m_type));
        return true;
    }
    return false;
}

Ability Character::canAttack(const Gameboard_t& board, const gf::Vector2i& origin, const gf::Vector2i& dest) const
{
    assert(board(origin));
    if (!board.isValid(dest)) {
        return Ability::Unable;
    }

    Ability result = Ability::Unable;
    gf::Vector2i relative = dest - origin;
    switch (m_type) {
    case CharacterType::Scout: {
        if (gf::manhattanDistance(origin, dest) <= 2) {
            gf::Vector2i direction = gf::sign(relative);
            result = (direction == relative || !board(origin + direction)) ? Ability::Able : Ability::Unavailable; // Can't attack through another character
        }
    } break;

    case CharacterType::Support: {
        constexpr int range = 3;

        if (isOrthogonal(origin, dest) && gf::chebyshevLength(relative) <= range) {
            result = isTargetReachable(board, origin, dest) ? Ability::Able : Ability::Unavailable;
        }
    } break;

    case CharacterType::Tank:
        if (gf::chebyshevDistance(origin, dest) == 1) {
            result = Ability::Able;
        }
    }

    if (result) {
        auto target = board(dest);
        return (target && target->m_team != m_team) ? Ability::Able : Ability::Unavailable;
    }

    return result;
}

bool Character::move(Gameboard_t& board, const gf::Vector2i& origin, const gf::Vector2i& dest)
{
    bool success{canMove(board, origin, dest)};

    if (success) {
        assert(board.isValid(origin));
        assert(board.isValid(dest));
        assert(dest == origin || !board(dest));

        boost::swap(board(origin), board(dest));
        board(dest)->m_pos = dest; // TODO Understand this sh*t
        m_pos = dest;
    }

    return success;
}

Ability Character::canMove(const Gameboard_t& board, const gf::Vector2i& origin, const gf::Vector2i& dest) const
{
    assert(board(origin));
    if (!board.isValid(dest)) {
        return Ability::Unable;
    }

    if (origin == dest) { // Don't move
        return Ability::Able;
    }

    gf::Vector2i relative = dest - origin;
    Ability result = Ability::Unable;

    switch (m_type) {
    case CharacterType::Scout: {
        constexpr int range = 4;

        if ((isOrthogonal(origin, dest) || isDiagonal(origin, dest)) && gf::chebyshevLength(relative) <= range) {
            result = isTargetReachable(board, origin, dest) ? Ability::Able : Ability::Unavailable;
        }
    } break;

    case CharacterType::Tank: {
        constexpr int sideRange = 2;

        if (gf::chebyshevDistance(origin, dest) == 1 || (relative.x == 0 && std::abs(relative.y) == sideRange)) {
            result = isTargetReachable(board, origin, dest) ? Ability::Able : Ability::Unavailable;
        }
    } break;

    case CharacterType::Support: {
        if (gf::manhattanDistance(origin, dest) == 3 && relative.x != 0 && relative.y != 0) {
            result = Ability::Able;
        }
    } break;
    }

    if (result) {
        if (board(dest)) { // If there is already a character at this location
            result = Ability::Unavailable;
        } else { // Check for nearby enemy Tank
            constexpr gf::Orientation cardinals[] = {gf::Orientation::North, gf::Orientation::East, gf::Orientation::South, gf::Orientation::West};
            for (auto card : cardinals) {
                gf::Vector2i sq2Check = origin + gf::displacement(card);
                if (board.isValid(sq2Check)) {
                    auto character2Check = board(sq2Check);
                    if (character2Check && character2Check->getTeam() == getEnemyTeam(m_team) &&
                        character2Check->getType() == CharacterType::Tank) {
                        result = Ability::Unavailable;
                        break;
                    }
                }
            }
        }
    }

    return result;
}

bool Character::useCapacity(Gameboard_t& board, const gf::Vector2i& origin, const gf::Vector2i& dest)
{
    if (!canUseCapacity(board, origin, dest)) {
        return false;
    }

    assert(board.isValid(origin));
    assert(board.isValid(dest));

    switch (m_type) {
    case CharacterType::Scout: {
        assert(!board(dest));

        boost::swap(board(origin), board(dest));
        board(dest)->m_pos = dest;
        m_pos = dest;
    } break;

    case CharacterType::Tank: {
        assert(board(dest));
        gf::Vector2i newPos = gf::sign(dest - origin);

        assert(!board(newPos));

        boost::swap(board(dest), board(newPos));
        board(newPos)->m_pos = newPos;
    } break;

    case CharacterType::Support: {
        constexpr int ejectionDistance = 2;
        constexpr int ejectionDamage = 3;

        assert(board(dest));

        gf::Vector2i ejectedPos = origin + ejectionDistance * gf::sign(dest - origin);
        gf::Vector2i lastReachablePos = getLastReachablePos(board, dest, ejectedPos);

        if (ejectedPos != lastReachablePos) {
            board(dest)->damage(ejectionDamage);
            ejectedPos = lastReachablePos;
        }

        assert(board.isValid(ejectedPos));
        assert(dest == ejectedPos || !board(ejectedPos));

        boost::swap(board(dest), board(ejectedPos));
        board(ejectedPos)->m_pos = ejectedPos;
    } break;
    }
    return true;
}

Ability Character::canUseCapacity(const Gameboard_t& board, const gf::Vector2i& origin, const gf::Vector2i& dest) const
{
    int i = 0;
    for (auto perso : board) {
        std::cout << (perso ? "X" : " ") << ((++i % 12 == 0) ? "\n" : " ");
    }
    std::cout << std::flush;

    assert(board(origin));
    if (!board.isValid(dest)) {
        return Ability::Unable;
    }

    int manhattanDist = gf::manhattanDistance(origin, dest);
    switch (m_type) {
    case CharacterType::Scout: {
        if (manhattanDist == 1) {
            return board(dest) ? Ability::Unavailable : Ability::Able;
        }
    } break;

    case CharacterType::Support: {
        if (isOrthogonal(origin, dest) && manhattanDist == 2) {
            return board(dest) ? Ability::Able : Ability::Unavailable;
        }
    } break;

    case CharacterType::Tank: {
        if (isOrthogonal(origin, dest) && (manhattanDist == 2 || manhattanDist == 3)) {
            return (board(dest) && isTargetReachable(board, origin, dest)) ? Ability::Able : Ability::Unavailable;
        }
    } break;
    }

    return Ability::Unable;
}

gf::Vector2i Character::getLastReachablePos(const Gameboard_t& board, const gf::Vector2i& origin,
                                            const gf::Vector2i& dest)
{
    if ((!isOrthogonal(origin, dest) && !isDiagonal(origin, dest)) || origin == dest) {
        return origin;
    }

    const gf::Vector2i direction = gf::sign(dest - origin);

    gf::Vector2i result = origin;
    gf::Vector2i sq2Check = result + direction;

    while (board.isValid(sq2Check) && !board(sq2Check)) {
        result = sq2Check;
        sq2Check += direction;
    }

    return result;
}

std::set<gf::Vector2i, PositionComp> Character::getAllPossibleActionsOfAType(const Gameboard_t& board,
                                                                             Ability (Character::* canDoSomething)(
                                                                                 const Gameboard_t&,
                                                                                 const gf::Vector2i&,
                                                                                 const gf::Vector2i&) const,
                                                                             bool usedForNotPossibleDisplay) const
{
    std::set<gf::Vector2i, PositionComp> res;
    for (int i = 0; i < board.getSize().x; ++i) {
        for (int j = 0; j < board.getSize().y; ++j) {
            gf::Vector2i pos{i, j};

            Ability possibleAction = (this->*canDoSomething)(board, m_pos, pos);
            if (usedForNotPossibleDisplay) {
                if (possibleAction != Ability::Unable) {
                    res.insert(pos);
                }
            } else if (possibleAction == Ability::Able) {
                res.insert(pos);
            }
        }
    }
    return res;
}
