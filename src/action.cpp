#include "action.h"

bool Action::isValid(gf::Array2D<boost::optional<Character>, int> board) const
{
    Character character{m_character};
    if (!character.canMove(m_move, board)) {
        return false;
    }

    gf::Vector2i previousPos{character.getPosition()};

    board(character.getPosition() + m_move);
    character.move(m_move, board);

    board(previousPos) = boost::none;
    board(character.getPosition()) = character;

    switch (m_type) {
    case ActionType::Capacity:
        return character.canUseCapacity(m_target - m_character.getPosition(), board);

    case ActionType::Attack:
        return character.canAttack(*board(m_target), board);

    case ActionType::None:
        break;
    }

    return true;
}

void Action::execute(gf::Array2D<boost::optional<Character>, int>& board)
{
    boost::optional<Character> oldP = board(m_character.getPosition());
    assert(oldP);
    board(m_character.getPosition()) = boost::none;
    oldP->move(m_move, board);
    m_character.move(m_move, board);
    board(oldP->getPosition()) = oldP;
    switch (m_type) {
    case ActionType::Capacity: {
        m_character.useCapacity(m_target, board);
    } break;

    case ActionType::Attack: {
        m_character.attack(*board(m_target), board);
    } break;

    case ActionType::None:
        break;
    }
}
