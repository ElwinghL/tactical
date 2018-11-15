#include "action.h"

bool Action::isValid(gf::Array2D<Character*, int> board) const
{
    Character character = Character(m_character.getTeam(), m_character.getType(), m_character.getPosition());
    
    if(!character.canMove(m_move,board)) {
        return false;
    }
    gf::Vector2i previousPos{character.getPosition()};
    board(character.getPosition()+m_move);
    character.move(m_move, board);
    board(previousPos) = nullptr;
    board(character.getPosition()) = &character;
    switch(m_type) {
        case ActionType::None: {
            return true;
            break;
        }
        case ActionType::Capacity: {
            return character.canUseCapacity(m_target,board);
            break;
        }
        case ActionType::Attack: {
            return character.canAttack(*board(m_target),board);
            break;
        }
    }
    return false;
}

void Action::execute()
{
}
