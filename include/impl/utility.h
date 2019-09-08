#ifndef IMPL_UTILITY_H
#define IMPL_UTILITY_H

constexpr PlayerTeam getEnemyTeam(PlayerTeam team)
{
    return (team == PlayerTeam::Cthulhu) ? PlayerTeam::Satan : PlayerTeam::Cthulhu;
}

constexpr bool isOrthogonal(const gf::Vector2i& v)
{
    return v.x == 0 || v.y == 0;
}

constexpr bool isOrthogonal(const gf::Vector2i& origin, const gf::Vector2i& dest)
{
    return isOrthogonal(dest - origin);
}

constexpr bool isDiagonal(const gf::Vector2i& v)
{
    return v.x == v.y || v.x == -v.y;
}

constexpr bool isDiagonal(const gf::Vector2i& origin, const gf::Vector2i& dest)
{
    return isDiagonal(dest - origin);
}

constexpr gf::Vector2f gameToScreenPos(const gf::Vector2f& gamePos)
{
    return gf::Vector2f{gamePos.y + gamePos.x, gamePos.y - gamePos.x - 1.0f} * halfTileSize;
}

constexpr bool PositionComp::operator()(const gf::Vector2i& lhs, const gf::Vector2i& rhs) const
{
    return std::tie(lhs.x, lhs.y) < std::tie(rhs.x, rhs.y);
}

#endif //IMPL_UTILITY_H
