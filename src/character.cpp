#include "character.h"

bool Character::canAttack(Character& other) const
{
    return false;
}

bool Character::canMove(const gf::Vector2i& movement, const gf::Array2D<Character*, int>& board) const
{
    if(movement.x == 0 && movement.y == 0){
        //Le personnage a le droit de ne pas se déplacer
        return true;
    }
    int absoluteX = movement.x + m_pos.x;
    int absoluteY = movement.y + m_pos.y;
    gf::Vector2i absolute{absoluteX,absoluteY};
    if(!board.isValid(absolute)){
        return false;
    }
    if(board(absolute) && board(absolute) != this){
        return false;
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

std::set<gf::Vector2i, PositionComp> Character::getAllPossibleMoves(gf::Array2D<Character*, int> board) const
{
    std::set<gf::Vector2i, PositionComp> res;
    for(int i = 0; i < board.getSize().x; ++i){
        for(int j = 0; j < board.getSize().y; ++j){
            gf::Vector2i pos{i,j};
            if(canMove(pos - m_pos, board)){
                res.insert(pos);
            }
        }
    }
    return res;
}

std::vector<Action> getPossibleActions()
{
    return std::vector<Action>{};
}
