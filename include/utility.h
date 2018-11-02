#ifndef UTILITY_H
#define UTILITY_H

#include <gf/Vector.h>

enum class PlayerTeam {
    Cthulhu,
    Satan
};

enum class ActionType {
    Attack,
    Capacity1,
    Capacity2,
    None
};

enum class CharacterType {
    Tank,
    Support,
    Scout
};

constexpr PlayerTeam getEnemyTeam(PlayerTeam team)
{
    return (team == PlayerTeam::Cthulhu) ? PlayerTeam::Satan : PlayerTeam::Cthulhu;
}

#endif // UTILITY_H
