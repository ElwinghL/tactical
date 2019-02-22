#include "gameboardview.h"

#include <gf/SpriteBatch.h>

GameboardView::GameboardView(const Gameboard& board, gf::ResourceManager& resMgr, gf::EntityContainer& entityMgr) :
    m_board{&board},
    m_resMgr{&resMgr},
    m_entityMgr{&entityMgr}
{
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
                auto createdEntity = std::make_unique<EntityCharacter>(*this, spr, c.getHP(), pos);
                m_entityMgr->addEntity(*createdEntity);
                m_entities.emplace_back(std::move(createdEntity));
            };

            putEntity(initSprite(c.getType(), c.getTeam()));
        }
    });

    m_darkTile.setAnchor(gf::Anchor::Center);
    m_brightTile.setAnchor(gf::Anchor::Center);

    m_magicLock.setAnchor(gf::Anchor::BottomCenter);
    m_magicLock.setOrigin(m_magicLock.getOrigin() - gf::Vector2f{0.f, 16.f});

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

void GameboardView::update()
{
    for (auto& entity : m_entities) {
        entity->setLocked(m_board->isLocked(entity->getDest()));
    }
}

bool GameboardView::animationFinished() const
{
    return true;
}

void GameboardView::notifyMove(const gf::Vector2i& origin, const gf::Vector2i& dest)
{
    auto it = std::find_if(m_entities.begin(), m_entities.end(), [&origin](auto& entity) {
        return entity->getDest() == origin;
    });

    assert(it != m_entities.end());
    (*it)->moveTo(dest);
}

void GameboardView::notifyHP(const gf::Vector2i& pos, int hp)
{
    auto it = std::find_if(m_entities.begin(), m_entities.end(), [&pos](auto& entity) {
        return entity->getDest() == pos;
    });

    assert(it != m_entities.end());

    if (hp <= 0) {
        m_entityMgr->removeTypedEntity(it->get());
        m_entities.erase(it);
    } else {
        (*it)->setHP(hp);
    }
}

void GameboardView::drawGrid(gf::RenderTarget& target, const gf::RenderStates& states)
{
    gf::SpriteBatch batch{target};
    batch.begin();

    m_board->forEach([this, &states, &batch](auto pos) {
        auto tileSpr = [this, &pos]() -> gf::Sprite& {
            gf::Sprite* res = nullptr;
            m_board->doWithGoals([this, &pos, &res](const Goal& goal) {
                if (goal.getPosition() == pos) {
                    auto& goalSprArr = (goal.getTeam() == PlayerTeam::Satan) ? m_goalSatan : m_goalCthulhu;
                    res = &goalSprArr[this->getActivationIndex(goal.isActivated())];
                }
            });

            if (res) {
                return *res;
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
    if (m_locked) {
        m_gbView->m_magicLock.setPosition(m_sprite.getPosition());
        m_gbView->m_magicLock.draw(target, states);
#ifdef SHOW_BOUNDING_BOXES
        showBoundingBox(m_gbView->m_magicLock, target, states);
#endif // SHOW_BOUNDING_BOXES
    }
}
