#include "utility.h"

#include <gf/VectorOps.h>

static constexpr gf::Vector2f posToIso(const gf::Vector2i& gamePos)
{
    return gf::Vector2f{
        static_cast<float>(gamePos.y + gamePos.x),
        static_cast<float>(gamePos.y - gamePos.x)
    };
}

gf::Vector2f positionToView(const gf::Vector2i& gamePos)
{
    constexpr gf::Vector2f tileSize{64.0f, 31.0f};

    gf::Vector2f res{posToIso(gamePos)};
    res.x *= tileSize.width / 2.0f;
    res.y *= tileSize.height / 2.0f;

    res.y -= tileSize.height / 2.0f;

    return res;
}

void resizeView(gf::View& view, const gf::Vector2i& gameSize)
{
    constexpr gf::Vector2f padding{50.0f, 50.0f};

    gf::Vector2f left{positionToView({0, 0})};
    gf::Vector2f right{positionToView(gameSize)};
    gf::Vector2f top{positionToView({gameSize.width, 0})};
    gf::Vector2f bottom{positionToView({0, gameSize.height})};

    view.setCenter({(left.x + right.x) / 2.0f, (top.y + bottom.y + padding.height) / 2.0f});
    view.setSize({right.x - left.x + padding.width, bottom.y - top.y + padding.height});
}
