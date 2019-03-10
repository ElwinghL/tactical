#include "gameboardview.h"

#include <gf/Easings.h>
#include <gf/SpriteBatch.h>

static constexpr gf::Vector2f getEasingPos(gf::Easing easing, const gf::Vector2f& origin, const gf::Vector2f& dest,
                                           float timeFrac)
{
    return gf::lerp(origin, dest, easing(timeFrac));
}

GameboardView::GameboardView(const Gameboard& board, gf::ResourceManager& resMgr, gf::EntityContainer& entityMgr) :
    m_board{&board},
    m_resMgr{&resMgr},
    m_entityMgr{&entityMgr}
{
    auto initSprite = [this](CharacterType type, PlayerTeam team) {
        std::string path{};
        switch (type) {
        case CharacterType::Tank: {
            if(team == PlayerTeam::Cthulhu){
                path = "cthulhu_tank_back_fixed.png";
            }else{
                path = "satan_tank_fixed.png";
            }
        }
            break;

        case CharacterType::Support: {
            if(team == PlayerTeam::Cthulhu){
                path = "cthulhu_support_back_fixed.png";
            }else{
                path = "satan_support_fixed.png";
            }
        }
            break;

        case CharacterType::Scout: {
            if(team == PlayerTeam::Cthulhu){
                path = "cthulhu_scout_back_fixed.png";
            }else{
                path = "satan_scout_fixed.png";
            }
        }
            break;
        }

        gf::Sprite spr{m_resMgr->getTexture(path)};
        spr.setAnchor(gf::Anchor::BottomCenter);
        spr.setOrigin(spr.getOrigin() - gf::Vector2f{0.f, 60.f});
        spr.setScale(0.25f);

        return spr;
    };

    board.forEach([this, &board, &initSprite](auto pos) {
        if (board.isOccupied(pos)) {
            Character c = board.getCharacter(pos);
            auto putEntity = [this, &pos, &c](gf::Sprite&& spr) {
                spr.setPosition(gameToScreenPos(pos));
                auto createdEntity = std::make_unique<EntityCharacter>(*this, spr, c.getHP(), pos);
                m_entityMgr->addEntity(*createdEntity);
                m_entities.emplace(pos, std::move(createdEntity));
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
        assert(entity.second);
        entity.second->setLocked(m_board->isOccupied(entity.first) && m_board->isLocked(entity.first));
    }
}

bool GameboardView::animationFinished() const
{
    return std::all_of(m_entities.begin(), m_entities.end(), [](auto& entity) {
        assert(entity.second);
        return entity.second->animationFinished();
    });
}

void GameboardView::notifyMove(const gf::Vector2i& origin, const gf::Vector2i& dest)
{
    assert(m_entities.count(origin) > 0);
    auto entity = std::move(m_entities[origin]);
    assert(entity);
    m_entities.erase(origin);
    entity->moveTo(dest);

    if (m_entities.count(dest) > 0) { // TODO better solution to swap characters
        auto otherEntity = std::move(m_entities[dest]);
        m_entities.erase(dest);
        otherEntity->moveTo(origin);
        m_entities.emplace(origin, std::move(otherEntity));
    }

    m_entities.emplace(dest, std::move(entity));
}

void GameboardView::notifyHP(const gf::Vector2i& pos, int hp)
{
    assert(m_entities.count(pos) > 0);
    assert(m_entities[pos]);
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

void GameboardView::EntityCharacter::update(gf::Time time)
{
    m_timeFrac += time.asSeconds() / 0.8f;
    m_timeFrac = gf::clamp(m_timeFrac, 0.0f, 1.0f);

    auto pos = getEasingPos(gf::Ease::smooth, m_origin, m_dest, m_timeFrac);
    setPriority(getPriorityFromPos(pos));
    m_sprite.setPosition(gameToScreenPos(pos));
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

void GameboardView::EntityCharacter::moveTo(const gf::Vector2i& pos)
{
    setPriority(getPriorityFromPos(pos));
    m_sprite.setPosition(gameToScreenPos(pos));

    m_origin = getEasingPos(gf::Ease::smooth, m_origin, m_dest, m_timeFrac);
    m_dest.x = pos.x;
    m_dest.y = pos.y;

    m_timeFrac = 0.0f;
}

void GameboardView::EntityCharacter::drawLife(gf::RenderTarget& target, const gf::RenderStates& states)
{
    auto& lifeSpr = m_gbView->m_lifeSprites[m_currentHPSprite];
    lifeSpr.setPosition(m_sprite.getPosition() + gf::Vector2f{0.f, -35.0f});
    lifeSpr.draw(target, states);
#ifdef SHOW_BOUNDING_BOXES
    showBoundingBox(lifeSpr, target, states);
#endif // SHOW_BOUNDING_BOXES
}
