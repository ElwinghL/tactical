#include "action.h"

#include <utility>

bool Action::isValid(gf::Array2D<boost::optional<Character>, int> board) const
{
    Character character{*board(m_characterPos)};
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
        return character.canUseCapacity(m_target - m_characterPos, board);

    case ActionType::Attack:
        return character.canAttack(*board(m_target), board);

    case ActionType::None:
        break;
    }

    return true;
}

void Action::execute(gf::Array2D<boost::optional<Character>, int>& board)
{
    gf::Vector2i oldPos = m_characterPos;
    boost::optional<Character>& character = board(m_characterPos);
    assert(character);
    character->move(m_move, board);
    switch (m_type) {
    case ActionType::Capacity: {
        character->useCapacity(m_target, board);
    } break;

    case ActionType::Attack: {
        character->attack(*board(m_target), board);
    } break;

    case ActionType::None:
        break;
    }
    m_characterPos = character->getPosition();
    std::swap(board(oldPos), board(m_characterPos));
}
