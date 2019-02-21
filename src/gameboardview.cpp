#include "gameboardview.h"

#include <gf/SpriteBatch.h>

GameboardView::GameboardView(const Gameboard& board, gf::ResourceManager& resMgr, gf::EntityContainer& entityMgr) :
    m_size{board.getSize()},
    m_resMgr{&resMgr},
    m_entityMgr{&entityMgr}
{
    std::size_t i = 0;
    board.doWithGoals([this, &i](const Goal& goal) {
        m_goals[i] = &goal;
        ++i;
    });

    auto initSprite = [this](CharacterType type, PlayerTeam team) {
        std::string path{};
        switch (type) {
        case CharacterType::Tank: {
            path = "tank.png";
        }
            break;

        case CharacterType::Support: {
            path = "support.png";
        }
            break;

        case CharacterType::Scout: {
            path = "scout.png";
        }
            break;
        }

        gf::Sprite spr{m_resMgr->getTexture(path)};
        spr.setAnchor(gf::Anchor::BottomCenter);
        spr.setOrigin(spr.getOrigin() - gf::Vector2f{0.f, 20.f});

        if (team == PlayerTeam::Cthulhu) {
            spr.setScale({-1.0f, 1.0f});
        } else {
            spr.setColor(gf::Color::Red);
        }

        return spr;
    };

    board.forEach([this, &board, &initSprite](auto pos) {
        if (board.isOccupied(pos)) {
            Character c = board.getCharacter(pos);
            auto putEntity = [this, &pos, &c](gf::Sprite&& spr) {
                spr.setPosition(gameToScreenPos(pos));
                auto createdEntity = std::make_unique<EntityCharacter>(*this, spr, c.getHP(), -(pos.x + 6 - pos.y));
                m_entityMgr->addEntity(*createdEntity);
                m_entities.emplace(pos, std::move(createdEntity));
            };

            putEntity(initSprite(c.getType(), c.getTeam()));
        }
    });

    m_darkTile.setAnchor(gf::Anchor::Center);
    m_brightTile.setAnchor(gf::Anchor::Center);

    for (auto& goal : m_goalCthulhu) {
        goal.setAnchor(gf::Anchor::Center);
    }

    for (auto& goal : m_goalSatan) {
        goal.setAnchor(gf::Anchor::Center);
    }

    for (auto& lifeSpr : m_lifeSprites) {
        lifeSpr.setAnchor(gf::Anchor::BottomCenter);
    }
}

bool GameboardView::animationFinished() const
{
    return true;
}

void GameboardView::notifyMove(const gf::Vector2i& origin, const gf::Vector2i& dest)
{
    auto entity = std::move(m_entities[origin]);
    assert(entity);
    m_entities.erase(origin);
    entity->moveTo(dest);
    m_entities.emplace(dest, std::move(entity));
}

void GameboardView::notifyHP(const gf::Vector2i& pos, int hp)
{
    if (hp <= 0) {
        m_entityMgr->removeTypedEntity(m_entities[pos].get());
        m_entities.erase(pos);
    } else {
        m_entities[pos]->setHP(hp);
    }
}

void GameboardView::drawGrid(gf::RenderTarget& target, const gf::RenderStates& states)
{
    gf::SpriteBatch batch{target};
    batch.begin();

    forEachPosition(m_size, [this, &states, &batch](auto pos) {
        auto tileSpr = [this, &pos]() -> gf::Sprite& {
            for (auto goal : m_goals) {
                if (goal->getPosition() == pos) {
                    auto goalSprArr = (goal->getTeam() == PlayerTeam::Satan) ? m_goalSatan : m_goalCthulhu;
                    return goalSprArr[this->getActivationIndex(goal->isActivated())];
                }
            }

            if ((pos.x + pos.y) % 2 == 0) {
                return m_darkTile;
            }

            return m_brightTile;
        }();

        tileSpr.setPosition(gameToScreenPos(pos));
        batch.draw(tileSpr, states);
    });

    batch.end();
}

void GameboardView::EntityCharacter::update(gf::Time /*time*/)
{

}

void GameboardView::EntityCharacter::render(gf::RenderTarget& target, const gf::RenderStates& states)
{
    m_sprite.draw(target, states);
#ifdef SHOW_BOUNDING_BOXES
    showBoundingBox(m_sprite, target, states);
#endif // SHOW_BOUNDING_BOXES
    drawLife(target, states);
}
