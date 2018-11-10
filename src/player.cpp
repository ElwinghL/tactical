#include "player.h"

bool Player::isOnAGoal(const Character& c) const
{
    return false;
}

Character* Player::getCharacter(gf::Vector2i pos)
{
    auto it = m_characters.find(pos);
    if(it == m_characters.end()){
        return NULL;
    }
    return &(it->second);
}
