#include "utility.h"

#include <gf/Color.h>
#include <gf/Shapes.h>
#include <gf/VectorOps.h>

#include <cmath>

static constexpr gf::Vector2f halfTileSize{64.0f / 2.0f, 31.0f / 2.0f};

void resizeView(gf::View& view, const gf::Vector2i& gameSize)
{
    constexpr gf::Vector2f padding{50.0f, 50.0f};

    gf::Vector2f left{gameToScreenPos({0, 0})};
    gf::Vector2f right{gameToScreenPos(gameSize)};
    gf::Vector2f top{gameToScreenPos({gameSize.width, 0})};
    gf::Vector2f bottom{gameToScreenPos({0, gameSize.height})};

    view.setCenter({(left.x + right.x) / 2.0f - halfTileSize.width, (top.y + bottom.y) / 2.0f});
    view.setSize({right.x - left.x + padding.width, bottom.y - top.y + padding.height});
}

static constexpr gf::Vector2f posToIso(const gf::Vector2i& gamePos)
{
    return gf::Vector2f{
            static_cast<float>(gamePos.y + gamePos.x),
            static_cast<float>(gamePos.y - gamePos.x)};
}

gf::Vector2f gameToScreenPos(const gf::Vector2i& gamePos)
{
    gf::Vector2f res{posToIso(gamePos)};
    res *= halfTileSize;
    res.y -= halfTileSize.height;

    return res;
}

static constexpr gf::Vector2i posToCart(const gf::Vector2f& pos)
{
    return gf::Vector2i{
            static_cast<int>(std::floor((pos.x - pos.y) / 2.0f)),
            static_cast<int>(std::floor((pos.x + pos.y) / 2.0f))};
}

gf::Vector2i screenToGamePos(const gf::Vector2f& screenPos)
{
    return posToCart((screenPos + halfTileSize) / halfTileSize);
}

#ifdef SHOW_BOUNDING_BOXES
void showBoundingBox(const gf::Sprite& sprite, gf::RenderTarget& target, const gf::RenderStates& states)
{
    gf::RectangleShape boundingBox{sprite.getLocalBounds()};
    boundingBox.setSize(boundingBox.getSize() - 4.0f);
    boundingBox.setColor(gf::Color::Transparent);
    boundingBox.setOutlineColor(gf::Color::White);
    boundingBox.setOutlineThickness(2.0f);
    boundingBox.setOrigin(sprite.getOrigin());
    boundingBox.setPosition(sprite.getPosition() + 2.0f);
    boundingBox.draw(target, states);

    gf::CircleShape origin{2.0f};
    origin.setColor(gf::Color::Red);
    origin.setAnchor(gf::Anchor::Center);
    origin.setPosition(sprite.getPosition());
    origin.draw(target, states);
}
#endif // SHOW_BOUNDING_BOXES
