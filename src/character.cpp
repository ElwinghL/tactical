#include "character.h"

#include "action.h"

std::set<gf::Vector2i, PositionComp> Character::getAllPossibleMoves(const Gameboard_t& board, bool usedForNotPossibleDisplay) const
{
    std::set<gf::Vector2i, PositionComp> res;
    for (int i = 0; i < board.getSize().x; ++i) {
        for (int j = 0; j < board.getSize().y; ++j) {
            gf::Vector2i pos{i, j};

            Ability possibleMove = canMove(board, m_pos, pos);
            if (usedForNotPossibleDisplay) {
                if (possibleMove != Ability::Unable) {
                    res.insert(pos);
                }
            } else if (possibleMove == Ability::Able) {
                res.insert(pos);
            }
        }
    }
    return res;
}

std::set<gf::Vector2i, PositionComp> Character::getAllPossibleCapacities(const Gameboard_t& board, bool usedForNotPossibleDisplay) const
{
    std::set<gf::Vector2i, PositionComp> res;
    for (int i = 0; i < board.getSize().x; ++i) {
        for (int j = 0; j < board.getSize().y; ++j) {
            gf::Vector2i pos{i, j};
            if (canUseCapacity(pos - m_pos, board, usedForNotPossibleDisplay)) {
                res.insert(pos);
            }
        }
    }
    return res;
}

std::set<gf::Vector2i, PositionComp> Character::getAllPossibleAttacks(const Gameboard_t& board, bool usedForNotPossibleDisplay) const
{
    std::set<gf::Vector2i, PositionComp> res;
    for (int i = 0; i < board.getSize().x; ++i) {
        for (int j = 0; j < board.getSize().y; ++j) {
            gf::Vector2i pos{i, j};

            Ability possibleAttack = canAttack(board, m_pos, pos);
            if (usedForNotPossibleDisplay) {
                if (possibleAttack != Ability::Unable) {
                    res.insert(pos);
                }
            } else if (possibleAttack == Ability::Able) {
                res.insert(pos);
            }
        }
    }
    return res;
}

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

        gf::Vector2i absRel{gf::abs(relative)};
        if ((absRel.x == 0 || absRel.y == 0) && gf::chebyshevLength(absRel) <= range) { // If vector is orthogonal
            result = isTargetReachable(board, origin, relative, range) ? Ability::Able : Ability::Unavailable;
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

        boost::swap(board(origin), board(dest));
        board(dest)->m_pos = dest; // TODO Understand this sh*t
    }

    return success;
}

Ability Character::canMove(const Gameboard_t& board, const gf::Vector2i& origin, const gf::Vector2i& dest) const
{
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
        gf::Vector2i absRel{gf::abs(relative)};

        if ((absRel.x == absRel.y || absRel.y == 0 || absRel.x == 0) && gf::chebyshevLength(absRel) <= range) {
            result = isTargetReachable(board, origin, relative, range) ? Ability::Able : Ability::Unavailable;
        }
    } break;

    case CharacterType::Tank: {
        constexpr int sideRange = 2;
        gf::Vector2i absRel{gf::abs(relative)};

        if (gf::chebyshevDistance(origin, dest) == 1 || (absRel.x == 0 && absRel.y == sideRange)) {
            result = isTargetReachable(board, origin, relative, sideRange) ? Ability::Able : Ability::Unavailable;
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

bool Character::useCapacity(gf::Vector2i& target, Gameboard_t& board)
{
    if (!canUseCapacity(gf::Vector2i{target - m_pos}, board)) {
        return false;
    }

    switch (m_type) {
    case CharacterType::Scout: {
        std::swap(board(m_pos), board(target));
        board(target)->m_pos = target;
        // m_pos = target not useful because this != board(target)->get_ptr()
        // so this character isn't useful anymore
    } break;

    case CharacterType::Tank: {
        gf::Vector2i relative = gf::Vector2i{target - m_pos};
        int xvalue = relative.x == 0 ? 0 : relative.x / abs(relative.x);
        int yvalue = relative.y == 0 ? 0 : relative.y / abs(relative.y);
        gf::Vector2i newPos = m_pos + gf::Vector2i{xvalue, yvalue};

        std::swap(board(target), board(newPos));
        board(newPos)->m_pos = newPos;
    } break;

    case CharacterType::Support: {
        gf::Vector2i relative = gf::Vector2i{target - m_pos};
        int xvalue = relative.x == 0 ? 0 : relative.x / abs(relative.x);
        int yvalue = relative.y == 0 ? 0 : relative.y / abs(relative.y);
        gf::Vector2i newPos = target + gf::Vector2i{xvalue, yvalue} * 2;
        if (!board.isValid(newPos) || board(newPos)) {
            newPos = target + gf::Vector2i{xvalue, yvalue};
            board(target)->damage(4);
            if (!board.isValid(newPos) || board(newPos)) {
                return true;
            }
        }
        std::swap(board(target), board(newPos));
        board(newPos)->m_pos = newPos;
    } break;
    }
    return true;
}

bool Character::canUseCapacity(const gf::Vector2i& target, const Gameboard_t& board, bool usedForNotPossibleDisplay) const
{
    switch (m_type) {
    case CharacterType::Scout: {
        if ((abs(target.x) == 1 && abs(target.y) == 0) || (abs(target.x) == 0 && abs(target.y) == 1)) {
            return usedForNotPossibleDisplay || !board(gf::Vector2i{target + m_pos});
        }
    } break;

    case CharacterType::Support: {
        if ((abs(target.x) == 2 && abs(target.y) == 0) || (abs(target.x) == 0 && abs(target.y) == 2)) {
            int absoluteX = target.x + m_pos.x;
            int absoluteY = target.y + m_pos.y;

            gf::Vector2i absolute{absoluteX, absoluteY};
            if (board.isValid(absolute) && board(absolute)) {
                return true;
            }

            return usedForNotPossibleDisplay;
        }
    } break;

    case CharacterType::Tank: {
        if ((abs(target.x) == 2 && abs(target.y) == 0) || (abs(target.x) == 3 && abs(target.y) == 0) || (abs(target.x) == 0 && abs(target.y) == 2) || (abs(target.x) == 0 && abs(target.y) == 3)) {
            int xvalue = target.x == 0 ? 0 : target.x / abs(target.x);
            int yvalue = target.y == 0 ? 0 : target.y / abs(target.y);
            gf::Vector2i direction{xvalue, yvalue};
            int factor = 1;
            while (direction * factor != target && factor < 3) {
                int absoluteX = (direction * factor).x + m_pos.x;
                int absoluteY = (direction * factor).y + m_pos.y;
                if (board(gf::Vector2i{absoluteX, absoluteY})) {
                    if (!usedForNotPossibleDisplay) {
                        return false;
                    }
                }
                factor++;
            }
            return usedForNotPossibleDisplay || board(gf::Vector2i{target + m_pos});
        }
    } break;
    }
    return false;
}
