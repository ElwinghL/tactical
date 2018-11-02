#ifndef ACTION_H
#define ACTION_H

#include "utility.h"

#include <gf/Vector.h>

class Character;

class Action {
public:
    Action(Character& character, ActionType type, const gf::Vector2i& move, const gf::Vector2i& target) :
        m_character{character},
        m_type{type},
        m_move{move},
        m_target{target}
    {
        // Nothing
    }

    Action(Character& character, const gf::Vector2i& move) :
        Action{character, ActionType::None, move, gf::Vector2i{0, 0}}
    {
        // Nothing
    }

    Action(Character& character, ActionType type, const gf::Vector2i& target) :
        Action{character, type, gf::Vector2i{0, 0}, target}
    {
        // Nothing
    }

    bool isValid() const;
    void execute();

private:
    Character& m_character;
    ActionType m_type;
    gf::Vector2i m_move;
    gf::Vector2i m_target;
};

#endif // ACTION_H
