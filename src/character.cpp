#include "character.h"

bool Character::canAttack(Character& other, const gf::Array2D<Character*, int>& board, bool usedForNotPossibleDisplay) const
{
    if(other.getTeam() == m_team){
        return false;
    }
    gf::Vector2i relative = other.getPosition() - m_pos;
    switch(m_type){
        case CharacterType::Scout :{
            if(abs(relative.x) <= 1 && abs(relative.y) <= 1){
                return true;
            }
            if((abs(relative.x) == 2 && abs(relative.y) == 0)||(abs(relative.y) == 2 && abs(relative.x) == 0)){
                int xvalue = relative.x == 0 ? 0 : relative.x/abs(relative.x);
                int yvalue = relative.y == 0 ? 0 : relative.y/abs(relative.y);
                gf::Vector2i direction{xvalue,yvalue};
                return usedForNotPossibleDisplay || !board(direction+m_pos);
            }
            return false;
            break;
        }
        case CharacterType::Support :{
            int xvalue = relative.x == 0 ? 0 : relative.x/abs(relative.x);
            int yvalue = relative.y == 0 ? 0 : relative.y/abs(relative.y);
            gf::Vector2i direction{xvalue,yvalue};
            if(xvalue != 0 && yvalue != 0){
                return false;
            }
            int factor = 1;
            while(direction*factor != relative && factor < 3){
                int absoluteX = (direction*factor).x + m_pos.x;
                int absoluteY = (direction*factor).y + m_pos.y;
                if(board(gf::Vector2i{absoluteX, absoluteY})){
                    return usedForNotPossibleDisplay;
                }
                factor++;
            }    
            return abs(relative.x) <= 3 && abs(relative.y) <= 3;
            break;
        }
        case CharacterType::Tank :{
            return abs(relative.x) <= 1 && abs(relative.y) <= 1;
            break;
        }
    }
    return true;
}

bool Character::attack(Character& other, const gf::Array2D<Character*, int>& board) const
{
    if(canAttack(other,board)){
        //TODO:Ajouter l'application des dégâts
        return true;
    }
    return false;
}

bool Character::canUseCapacity(const gf::Vector2i& target, const gf::Array2D<Character*, int>& board, bool usedForNotPossibleDisplay) const
{
    switch(m_type){
        case CharacterType::Scout: {
            if((abs(target.x) == 1 && abs(target.y) == 0) || (abs(target.x) == 0 && abs(target.y) == 1)){
                return usedForNotPossibleDisplay || !board(gf::Vector2i{target + m_pos});
            }
            return false;
            break;
        }
        case CharacterType::Support: {
            if((abs(target.x) == 2 && abs(target.y) == 0) || (abs(target.x) == 0 && abs(target.y) == 2)){
                int absoluteX = target.x + m_pos.x;
                int absoluteY = target.y + m_pos.y;
                gf::Vector2i absolute{absoluteX,absoluteY};
                if(board.isValid(absolute) && board(absolute)){
                    return true;
                }
                return usedForNotPossibleDisplay;
            }
            return false;
            break;
        }
        case CharacterType::Tank: {
            if((abs(target.x) == 2 && abs(target.y) == 0) || (abs(target.x) == 3 && abs(target.y) == 0) || (abs(target.x) == 0 && abs(target.y) == 2) || (abs(target.x) == 0 && abs(target.y) == 3)){
                
                int xvalue = target.x == 0 ? 0 : target.x/abs(target.x);
                int yvalue = target.y == 0 ? 0 : target.y/abs(target.y);
                gf::Vector2i direction{xvalue,yvalue};
                int factor = 1;
                while(direction*factor != target && factor < 3){
                    int absoluteX = (direction*factor).x + m_pos.x;
                    int absoluteY = (direction*factor).y + m_pos.y;
                    if(board(gf::Vector2i{absoluteX, absoluteY})){
                        return usedForNotPossibleDisplay;
                    }
                    factor++;
                }
                return usedForNotPossibleDisplay || board(gf::Vector2i{target+m_pos});
            }
            return false;
            break;
        }
    }
    return false;
}

bool Character::canMove(const gf::Vector2i& movement, const gf::Array2D<Character*, int>& board, bool usedForNotPossibleDisplay) const
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
    if(!usedForNotPossibleDisplay && board(absolute) && board(absolute) != this){
        return false;
    }
    switch(m_type){
        case CharacterType::Scout:{
            if(movement.x == 0 || movement.y == 0 || abs(movement.x) == abs(movement.y)){
                int xvalue = movement.x == 0 ? 0 : movement.x/abs(movement.x);
                int yvalue = movement.y == 0 ? 0 : movement.y/abs(movement.y);
                gf::Vector2i direction{xvalue,yvalue};
                int factor = 1;
                while(direction*factor != movement && factor < 4){
                    absoluteX = (direction*factor).x + m_pos.x;
                    absoluteY = (direction*factor).y + m_pos.y;
                    if(board(gf::Vector2i{absoluteX, absoluteY})){
                        return usedForNotPossibleDisplay;
                    }
                    factor++;
                }
                return abs(movement.x) <= 4 && abs(movement.y) <= 4;
            }
            return false;
        }
        case CharacterType::Tank:{
            if(abs(movement.x) == 0 && abs(movement.y) == 2){
                absoluteX = m_pos.x;
                absoluteY = m_pos.y + movement.y/2;
                return usedForNotPossibleDisplay || !board(gf::Vector2i{absoluteX,absoluteY});
            }
            return (abs(movement.x) <= 1 && abs(movement.y) <= 1);
        }
        case CharacterType::Support:{
            return ((abs(movement.x) == 1 && abs(movement.y) == 2) || (abs(movement.x) == 2 && abs(movement.y) == 1));
        }
    }
    return true;
}

std::set<gf::Vector2i, PositionComp> Character::getAllPossibleMoves(const gf::Array2D<Character*, int>& board, bool usedForNotPossibleDisplay) const
{
    std::set<gf::Vector2i, PositionComp> res;
    for(int i = 0; i < board.getSize().x; ++i){
        for(int j = 0; j < board.getSize().y; ++j){
            gf::Vector2i pos{i,j};
            if(canMove(pos - m_pos, board, usedForNotPossibleDisplay)){
                res.insert(pos);
            }
        }
    }
    return res;
}

std::set<gf::Vector2i, PositionComp> Character::getAllPossibleCapacities(const gf::Array2D<Character*, int>& board, bool usedForNotPossibleDisplay) const
{
    std::set<gf::Vector2i, PositionComp> res;
    for(int i = 0; i < board.getSize().x; ++i){
        for(int j = 0; j < board.getSize().y; ++j){
            gf::Vector2i pos{i,j};
            if(canUseCapacity(pos - m_pos, board, usedForNotPossibleDisplay)){
                res.insert(pos);
            }
        }
    }
    return res;
}

std::set<gf::Vector2i, PositionComp> Character::getAllPossibleAttacks(const gf::Array2D<Character*, int>& board, bool usedForNotPossibleDisplay) const
{
    std::set<gf::Vector2i, PositionComp> res;
    for(int i = 0; i < board.getSize().x; ++i){
        for(int j = 0; j < board.getSize().y; ++j){
            gf::Vector2i pos{i,j};
            Character *target = board(pos);
            if(usedForNotPossibleDisplay){
                //On simule une cible sur la case pour avoir toutes les possibilités lors de l'affichage
                Character simulateCharacter{getEnemyTeam(m_team),CharacterType::Scout,gf::Vector2i{i,j}};
                target = &simulateCharacter;
            }
            if(target && canAttack(*target, board, usedForNotPossibleDisplay)){
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
