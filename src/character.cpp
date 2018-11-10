#include "character.h"

bool Character::canAttack(Character& other) const
{
    return false;
}

bool Character::canMove(const gf::Vector2i& movement) const
{
    return true;
}

std::vector<Action> getPossibleActions()
{
    return std::vector<Action>{};
}
