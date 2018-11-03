#include "utility.h"

#include <gf/VectorOps.h>

#include <cmath>

void resizeView(gf::View& view, const gf::Vector2i& gameSize)
{
    constexpr gf::Vector2f padding{50.0f, 50.0f};

    gf::Vector2f left{gameToScreenPos({0, 0})};
    gf::Vector2f right{gameToScreenPos(gameSize)};
    gf::Vector2f top{gameToScreenPos({gameSize.width, 0})};
    gf::Vector2f bottom{gameToScreenPos({0, gameSize.height})};

    view.setCenter({(left.x + right.x) / 2.0f, (top.y + bottom.y + padding.height) / 2.0f});
    view.setSize({right.x - left.x + padding.width, bottom.y - top.y + padding.height});
}

static constexpr gf::Vector2f halfTileSize{64.0f / 2.0f, 31.0f / 2.0f};

static constexpr gf::Vector2f posToIso(const gf::Vector2i& gamePos)
{
    return gf::Vector2f{
        static_cast<float>(gamePos.y + gamePos.x),
        static_cast<float>(gamePos.y - gamePos.x)
    };
}

gf::Vector2f gameToScreenPos(const gf::Vector2i& gamePos)
{
    gf::Vector2f res{posToIso(gamePos)};
    res.x *= halfTileSize.width;
    res.y *= halfTileSize.height;

    res.y -= halfTileSize.height;

    return res;
}

static constexpr gf::Vector2i posToCart(const gf::Vector2f& pos)
{
    return gf::Vector2i{
        static_cast<int>(std::floor((pos.x - pos.y) / 2.0f)),
        static_cast<int>(std::floor((pos.x + pos.y) / 2.0f))
    };
}

gf::Vector2i screenToGamePos(gf::Vector2f screenPos)
{
    screenPos.x /= halfTileSize.width;
    screenPos.y /= halfTileSize.height;

    return posToCart(screenPos);
}
