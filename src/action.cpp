#include "action.h"

#include <utility>

bool Action::isValid(Gameboard_t board) const
{
    assert(board(m_origin));
    Character& character = *board(m_origin);
    if (!character.canMove(m_dest - m_origin, board)) {
        return false;
    }

    character.move(m_dest - m_origin, board);

    std::swap(board(m_dest), board(m_origin));

    switch (m_type) {
    case ActionType::Capacity:
        return character.canUseCapacity(m_target - m_dest, board);

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

    characterBeforeMove->move(m_dest - m_origin, board);
    std::swap(board(m_dest), board(m_origin));

    boost::optional<Character>& character = board(m_dest);
    assert(character);
    switch (m_type) {
    case ActionType::Capacity: {
        character->useCapacity(m_target, board);
    } break;

    case ActionType::Attack: {
        character->attack(board, m_origin, m_target);
    } break;

    case ActionType::None:
        break;
    }
}
