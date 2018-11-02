#ifndef UTILITY_H
#define UTILITY_H

#include <gf/Vector.h>

constexpr char gameName[]{"Cthulhu vs Satan"};
constexpr gf::Vector2i gameSize{12, 6};

enum class PlayerTeam {
    Cthulhu,
    Satan
};

enum class CharacterType {
    Tank,
    Support,
    Scout
};

constexpr bool positionIsValid(const gf::Vector2i& pos)
{
    return pos.x >= 0 && pos.y >= 0 && pos.x < gameSize.width && pos.y < gameSize.height;
}

constexpr PlayerTeam getEnemyTeam(PlayerTeam team)
{
    return (team == PlayerTeam::Cthulhu) ? PlayerTeam::Satan : PlayerTeam::Cthulhu;
}

#endif // UTILITY_H
