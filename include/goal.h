#ifndef GOAL_H
#define GOAL_H

#include "utility.h"

#include <gf/Vector.h>

#include <cassert>

class Goal {
public:
    Goal(PlayerTeam team, const gf::Vector2i& pos) :
        m_team{team},
        m_pos{pos}
    {
        // Nothing
    }

    PlayerTeam getTeam() const
    {
        return m_team;
    }

    gf::Vector2i getPosition() const
    {
        return m_pos;
    }

private:
    PlayerTeam m_team;
    gf::Vector2i m_pos;
};

#endif // GOAL_H
