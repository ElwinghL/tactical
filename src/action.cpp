#include "action.h"

#include <utility>

bool Action::isValid(Gameboard_t board) const
{
    assert(board(m_characterPos));
    Character& character = *board(m_characterPos);
    if (!character.canMove(m_move, board)) {
        return false;
    }

    character.move(m_move, board);
    gf::Vector2i newPos{m_characterPos + m_move};

    std::swap(board(newPos), board(m_characterPos));

    switch (m_type) {
    case ActionType::Capacity:
        return character.canUseCapacity(m_target - newPos, board);

    case ActionType::Attack:
        return character.canAttack(*board(m_target), board);

    case ActionType::None:
        break;
    }

    return true;
}

void Action::execute(Gameboard_t& board)
{
    gf::Vector2i oldPos = m_characterPos;
    boost::optional<Character>& characterBeforeMove = board(m_characterPos);
    assert(characterBeforeMove);

    characterBeforeMove->move(m_move, board);
    m_characterPos = characterBeforeMove->getPosition();
    std::swap(board(oldPos), board(m_characterPos));

    boost::optional<Character>& character = board(m_characterPos);
    assert(character);
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
}
