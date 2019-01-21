#include "character.h"

#include "action.h"

#include <gf/Orientation.h>
#include <gf/VectorOps.h>

std::set<gf::Vector2i, PositionComp> Character::getAllPossibleMoves(const Gameboard_t& board, bool usedForNotPossibleDisplay) const
{
    std::set<gf::Vector2i, PositionComp> res;
    for (int i = 0; i < board.getSize().x; ++i) {
        for (int j = 0; j < board.getSize().y; ++j) {
            gf::Vector2i pos{i, j};
            if (canMove(pos - m_pos, board, usedForNotPossibleDisplay)) {
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
        res.emplace_back(ActionType::None, m_pos, possibleMovement);

        Gameboard_t thisBoard{board};
        Character character{*this};
        thisBoard(character.getPosition()) = boost::none;
        thisBoard(possibleMovement) = character;
        character.move(possibleMovement - character.getPosition(), thisBoard);

        std::set<gf::Vector2i, PositionComp> possibleCapacities = character.getAllPossibleCapacities(thisBoard);
        for (auto possibleCapacity : possibleCapacities) {
            res.emplace_back(ActionType::Capacity, m_pos, possibleMovement, possibleCapacity);
        }

        std::set<gf::Vector2i, PositionComp> possibleAttacks = character.getAllPossibleAttacks(thisBoard);
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

Ability Character::canAttack(const Gameboard_t& board, const gf::Vector2i& origin,
                                           const gf::Vector2i& dest) const
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
        gf::Vector2i xUnit = gf::displacement(gf::Orientation::East);
        if (gf::cross(xUnit, relative) == 0 || gf::dot(xUnit, relative) == 0) { // If vector is orthogonal
            result = Ability::Unavailable;

            // Check if there is a character in this direction
            gf::Vector2i direction = gf::sign(relative);
            for (gf::Vector2i sq2Check = origin + direction, maxRange = origin + 3 * direction;
                 sq2Check != maxRange;
                 sq2Check += direction) {
                if (sq2Check == dest) {
                    result = Ability::Able;
                    break;
                }

                if (board(sq2Check)) {
                    break;
                }
            }
        }
    }

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

bool Character::move(const gf::Vector2i& movement, const Gameboard_t& board)
{
    bool success{canMove(movement, board)};

    if (success) {
        m_pos += movement;
    }

    return success;
}

bool Character::canMove(const gf::Vector2i& movement, const Gameboard_t& board, bool usedForNotPossibleDisplay) const
{
    if (movement.x == 0 && movement.y == 0) {
        //Le personnage a le droit de ne pas se déplacer
        return true;
    }
    int absoluteX = movement.x + m_pos.x;
    int absoluteY = movement.y + m_pos.y;
    gf::Vector2i absolute{absoluteX, absoluteY};
    if (!board.isValid(absolute)) {
        return false;
    }
    for (int i = 0; i < 4; ++i) {
        int a = (i == 3) ? -1 : i / 2;
        int b = (i == 1) ? -1 : (2 - i) / 2;
        if (board.isValid(m_pos + gf::Vector2i{a, b}) && board(m_pos + gf::Vector2i{a, b}) && board(m_pos + gf::Vector2i{a, b})->getType() == CharacterType::Tank && board(m_pos + gf::Vector2i{a, b})->getTeam() != m_team) {
            if (!usedForNotPossibleDisplay) {
                return false;
            }
        }
    }
    //On ne peut pas se déplacer sur un autre personnage :
    if (!usedForNotPossibleDisplay && board(absolute)) {
        return false;
    }
    switch (m_type) {
    case CharacterType::Scout: {
        if (movement.x == 0 || movement.y == 0 || abs(movement.x) == abs(movement.y)) {
            int xvalue = movement.x == 0 ? 0 : movement.x / abs(movement.x);
            int yvalue = movement.y == 0 ? 0 : movement.y / abs(movement.y);
            gf::Vector2i direction{xvalue, yvalue};
            int factor = 1;
            while (direction * factor != movement && factor < 4) {
                absoluteX = (direction * factor).x + m_pos.x;
                absoluteY = (direction * factor).y + m_pos.y;
                if (board(gf::Vector2i{absoluteX, absoluteY}) && !usedForNotPossibleDisplay) {
                    return false;
                }
                factor++;
            }
            return abs(movement.x) <= 4 && abs(movement.y) <= 4;
        }
        return false;
    }
    case CharacterType::Tank: {
        if (abs(movement.x) == 0 && abs(movement.y) == 2) {
            absoluteX = m_pos.x;
            absoluteY = m_pos.y + movement.y / 2;
            return usedForNotPossibleDisplay || !board(gf::Vector2i{absoluteX, absoluteY});
        }
        return (abs(movement.x) <= 1 && abs(movement.y) <= 1);
    }
    case CharacterType::Support: {
        return ((abs(movement.x) == 1 && abs(movement.y) == 2) || (abs(movement.x) == 2 && abs(movement.y) == 1));
    }
    }
    return true;
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
