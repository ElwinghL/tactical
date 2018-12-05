#include "action.h"

bool Action::isValid(gf::Array2D<Character*, int> board) const
{
    Character character{m_character};
    if (!character.canMove(m_move, board)) {
        return false;
    }

    gf::Vector2i previousPos{character.getPosition()};

    board(character.getPosition() + m_move);
    character.move(m_move, board);

    board(previousPos) = nullptr;
    board(character.getPosition()) = &character;

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

void Action::execute(gf::Array2D<Character*, int>& board)
{
    Character* oldP = board(m_character.getPosition());

    board(m_character.getPosition()) = nullptr;
    m_character.move(m_move, board);

    board(m_character.getPosition()) = oldP;

    switch (m_type) {
    case ActionType::Capacity: {
        switch (m_character.getType()) {
        case CharacterType::Scout: {
            oldP = board(m_character.getPosition());
            board(m_character.getPosition()) = nullptr;
            m_character.useCapacity(m_target, board);
            board(m_character.getPosition()) = oldP;
        } break;

        case CharacterType::Tank: {
            oldP = board(m_target);
            m_character.useCapacity(m_target, board);
            board(m_target) = nullptr;
            board(oldP->getPosition()) = oldP;
        } break;

        case CharacterType::Support: {
            oldP = board(m_target);
            m_character.useCapacity(m_target, board);
            board(m_target) = nullptr;
            board(oldP->getPosition()) = oldP;
        } break;
        } // TODO Refactor or add something
    } break;

    case ActionType::Attack: {
        m_character.attack(*board(m_target), board);
    } break;

    case ActionType::None:
        break;
    }
}
