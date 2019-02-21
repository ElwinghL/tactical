#include <utility>

/**
 * Utilities to draw a Gameboard
 * \author Fabien Matusalem
 * \date 16 Feb 2019
 */

#ifndef CTHULHUVSSATAN_GAMEBOARDVIEW_H
#define CTHULHUVSSATAN_GAMEBOARDVIEW_H

#include "character.h"
#include "gameboard.h"
#include "utility.h"

#include <gf/Entity.h>
#include <gf/EntityContainer.h>
#include <gf/ResourceManager.h>
#include <gf/Sprite.h>
#include <gf/Time.h>

#include <array>
#include <memory>

#include <cassert>

class Action;

class Goal;

class GameboardView {
public:
    explicit GameboardView(const Gameboard& board, gf::ResourceManager& resMgr, gf::EntityContainer& entityMgr);

    bool animationFinished() const;

    void drawGrid(gf::RenderTarget& target, const gf::RenderStates& states = gf::RenderStates{});

    void notifyMove(const gf::Vector2i& origin, const gf::Vector2i& dest);

    void notifyHP(const gf::Vector2i& pos, int hp);

private:
    class EntityCharacter : public gf::Entity {
    public:
        explicit EntityCharacter(GameboardView& gbView, gf::Sprite sprite, int hp, int priority = 0) :
            Entity{priority},
            m_gbView{&gbView},
            m_sprite{std::move(sprite)},
            m_currentHPSprite{checkHP(hp)}
        {
            // Nothing
        }

        void update(gf::Time time) override;

        void render(gf::RenderTarget& target, const gf::RenderStates& states) override;

        void setHP(int hp)
        {
            m_currentHPSprite = checkHP(hp);
        }

        void moveTo(const gf::Vector2i& pos)
        {
            setPriority(-(pos.x + 6 - pos.y));
            m_sprite.setPosition(gameToScreenPos(pos));
        }

    private:
        std::size_t checkHP(int hp)
        {
            assert(hp > 0);
            return static_cast<std::size_t>(hp - 1);
        }

        void drawLife(gf::RenderTarget& target, const gf::RenderStates& states)
        {
            auto& lifeSpr = m_gbView->m_lifeSprites[m_currentHPSprite];
            lifeSpr.setPosition(m_sprite.getPosition() + gf::Vector2f{0.f, 16.f - m_sprite.getOrigin().y});
            lifeSpr.draw(target, states);
#ifdef SHOW_BOUNDING_BOXES
            showBoundingBox(lifeSpr, target, states);
#endif // SHOW_BOUNDING_BOXES
        }

        GameboardView* m_gbView;
        gf::Sprite m_sprite;

        std::size_t m_currentHPSprite;
    };

    std::size_t getActivationIndex(bool activated) const
    {
        return activated ? 1 : 0;
    }

    gf::Vector2i m_size;
    std::array<const Goal*, Gameboard::goalsPerTeam * 2> m_goals{};

    gf::ResourceManager* m_resMgr;
    gf::EntityContainer* m_entityMgr;

    gf::Sprite m_darkTile{m_resMgr->getTexture("case.png")};
    gf::Sprite m_brightTile{m_resMgr->getTexture("case2.png")};

    std::array<gf::Sprite, 2> m_goalCthulhu{
        m_resMgr->getTexture("caseGoalCthulhu.png"),
        m_resMgr->getTexture("caseGoalCthulhuActivated.png")
    };

    std::array<gf::Sprite, 2> m_goalSatan{
        m_resMgr->getTexture("caseGoalSatan.png"),
        m_resMgr->getTexture("caseGoalSatanActivated.png")
    };

    std::array<gf::Sprite, Character::getGlobalHPMax()> m_lifeSprites{
        m_resMgr->getTexture("life1.png"),
        m_resMgr->getTexture("life2.png"),
        m_resMgr->getTexture("life3.png"),
        m_resMgr->getTexture("life4.png"),
        m_resMgr->getTexture("life5.png"),
        m_resMgr->getTexture("life6.png"),
        m_resMgr->getTexture("life7.png"),
        m_resMgr->getTexture("life8.png")
    };

    std::map<gf::Vector2i, std::unique_ptr<EntityCharacter>, PositionComp> m_entities{};
};

#endif //CTHULHUVSSATAN_GAMEBOARDVIEW_H
