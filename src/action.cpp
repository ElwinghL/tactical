#include "action.h"

#include "gameboard.h"

#include <iostream>

[[nodiscard]] bool Action::isValid(const Gameboard& board) const
{
    if (!board.isOccupied(m_origin) || !board.canMove(m_origin, m_dest)) {
        return false;
    }

    switch (m_type) {
    case ActionType::Capacity:
        return board.canUseCapacity(m_dest, m_target, m_origin);

    case ActionType::Attack:
        return board.canAttack(m_dest, m_target, m_origin);

    case ActionType::None:
        break;
    }

    return true;
}

void Action::execute(Gameboard& board) const
{
    board.move(m_origin, m_dest);

    switch (m_type) {
    case ActionType::Capacity: {
        board.useCapacity(m_dest, m_target);
    } break;

    case ActionType::Attack: {
        board.attack(m_dest, m_target);
    } break;

    case ActionType::None:
        break;
    }
}

void Action::display() const
{
    if (m_origin != m_dest) {
        std::cout << "Move from (" << m_origin.x << ", " << m_origin.y << ") to (" << m_dest.x << ", " << m_dest.y
                  << ") then ";
    } else {
        std::cout << "From (" << m_origin.x << ", " << m_origin.y << "), ";
    }

    switch (m_type) {
    case ActionType::Attack:
        std::cout << "attack (" << m_target.x << ", " << m_target.y << ")" << std::endl;
        break;

    case ActionType::Capacity:
        std::cout << "use capacity on (" << m_target.x << ", " << m_target.y << ")" << std::endl;
        break;

    case ActionType::None:
        std::cout << "do nothing" << std::endl;
        break;
    }
}
