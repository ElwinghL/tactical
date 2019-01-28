#include "action.h"

bool Action::isValid(Gameboard_t board) const
{
    assert(board(m_origin));
    Character& character = *board(m_origin);
    if (!character.canMove(board, m_origin, m_dest)) {
        return false;
    }

    character.move(board, m_origin, m_dest);

    switch (m_type) {
    case ActionType::Capacity:return character.canUseCapacity(board, m_dest, m_target);

    case ActionType::Attack:
        return character.canAttack(board, m_dest, m_target);

    case ActionType::None:
        break;
    }

    return true;
}

void Action::execute(Gameboard_t& board)
{
    boost::optional<Character>& characterBeforeMove = board(m_origin);
    assert(characterBeforeMove);

    characterBeforeMove->move(board, m_origin, m_dest);

    boost::optional<Character>& character = board(m_dest);
    assert(character);
    switch (m_type) {
    case ActionType::Capacity: {
        character->useCapacity(board, m_origin, m_target);
    } break;

    case ActionType::Attack: {
        character->attack(board, m_origin, m_target);
    } break;

    case ActionType::None:
        break;
    }
}
