#include "action.h"

#include "gameboard.h"

bool Action::isValid(Gameboard board) const
{
    assert(board.isOccupied(m_origin));
    if (!board.canMove(m_origin, m_dest)) {
        return false;
    }

    switch (m_type) {
    case ActionType::Capacity:return board.canUseCapacity(m_dest, m_target);

    case ActionType::Attack:return board.canAttack(m_dest, m_target);

    case ActionType::None:
        break;
    }

    return true;
}

void Action::execute(Gameboard& board)
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
