#include "character.h"

bool Character::canAttack(Character& other) const
{
    return false;
}

bool Character::canMove(const gf::Vector2i& movement) const
{
    if(movement.x == 0 && movement.y == 0){
        //Le personnage a le droit de ne pas se déplacer
        return true;
    }
    switch(m_type){
        case CharacterType::Scout:{
            if(movement.x == 0 || movement.y == 0 || abs(movement.x) == abs(movement.y)){
                return abs(movement.x) <= 4 && abs(movement.y) <= 4;
            }
            return false;
        }
        case CharacterType::Tank:{
            return ((abs(movement.x) <= 1 && abs(movement.y) <= 1) || (abs(movement.y) == 2 && movement.x == 0));
        }
        case CharacterType::Support:{
            return ((abs(movement.x) == 1 && abs(movement.y) == 2) || (abs(movement.x) == 2 && abs(movement.y) == 1));
        }
    }
    return true;
}

std::vector<Action> getPossibleActions()
{
    return std::vector<Action>{};
}
