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
#include <gf/AnimatedSprite.h>
#include <gf/Animation.h>

#include <gf/Time.h>
#include <array>
#include <map>

#include <memory>
#include <cassert>

class Action;

class Goal;

constexpr int getPriorityFromPos(const gf::Vector2i& pos)
{
    return pos.y - pos.x - 6; // TODO magic constant
}

constexpr int getPriorityFromPos(const gf::Vector2f& pos)
{
    return static_cast<int>(pos.y - pos.x) - 6;
}

class GameboardView {
public:
    explicit GameboardView(const Gameboard& board, gf::ResourceManager& resMgr, gf::EntityContainer& entityMgr);

    bool animationFinished() const;

    void drawGrid(gf::RenderTarget& target, const gf::RenderStates& states = gf::RenderStates{});

    void update();

    void notifyMove(const gf::Vector2i& origin, const gf::Vector2i& dest);

    void notifyHP(const gf::Vector2i& pos, int hp);

private:
    class EntityCharacter : public gf::Entity {
    public:
        explicit EntityCharacter(GameboardView& gbView, gf::AnimatedSprite sprite, int hp, const gf::Vector2i& pos) :
            Entity{getPriorityFromPos(pos)},
            m_gbView{&gbView},
            m_sprite{std::move(sprite)},
            m_origin{static_cast<float>(pos.x), static_cast<float>(pos.y)},
            m_dest{m_origin},
            m_timeFrac{1.0f},
            m_currentHPSprite{checkHP(hp)}
        {
            m_currentAnimation = nullptr;
        }

        void update(gf::Time time) override;

        void render(gf::RenderTarget& target, const gf::RenderStates& states) override;

        void setHP(int hp)
        {
            m_currentHPSprite = checkHP(hp);
        }

        void moveTo(const gf::Vector2i& pos);

        void setLocked(bool locked)
        {
            m_locked = locked;
        }

        bool animationFinished() const
        {
            return gf::almostEquals(m_timeFrac, 1.0f);
        }
        
        void setCurrentAnimation(gf::Animation *animPtr)
        {
            m_currentAnimation = animPtr;
        }

    private:
        std::size_t checkHP(int hp)
        {
            assert(hp > 0);
            return static_cast<std::size_t>(hp - 1);
        }

        void drawLife(gf::RenderTarget& target, const gf::RenderStates& states);

        GameboardView* m_gbView;
        gf::AnimatedSprite m_sprite;

        gf::Vector2f m_origin;
        gf::Vector2f m_dest;

        float m_timeFrac;

        std::size_t m_currentHPSprite;

        bool m_locked{false};
        
        gf::Animation *m_currentAnimation;
    };

    std::size_t getActivationIndex(bool activated) const
    {
        return activated ? 1 : 0;
    }
    
    void initAnimations()
    {
        std::string path = "characters/cthulhu_tank_back.png";
        m_animCthulhuTank = gf::Animation{};
        for(std::size_t i = 0; i < 6; ++i){
            gf::RectF bounds{i * 256.0f,0.0f,256.0f,256.0f};
            m_animCthulhuTank.addFrame(m_resMgr->getTexture(path), bounds, gf::milliseconds(200));
        }
        path = "characters/satan_tank.png";
        m_animSatanTank = gf::Animation{};
        for(std::size_t i = 0; i < 6; ++i){
            gf::RectF bounds{i * 256.0f,0.0f,256.0f,256.0f};
            m_animSatanTank.addFrame(m_resMgr->getTexture(path), bounds, gf::milliseconds(200));
        }
        path = "characters/cthulhu_support_back.png";
        m_animCthulhuSupport = gf::Animation{};
        for(std::size_t i = 0; i < 6; ++i){
            gf::RectF bounds{i * 256.0f,0.0f,256.0f,256.0f};
            m_animCthulhuSupport.addFrame(m_resMgr->getTexture(path), bounds, gf::milliseconds(200));
        }
        path = "characters/satan_support.png";
        m_animSatanSupport = gf::Animation{};
        for(std::size_t i = 0; i < 6; ++i){
            gf::RectF bounds{i * 256.0f,0.0f,256.0f,256.0f};
            m_animSatanSupport.addFrame(m_resMgr->getTexture(path), bounds, gf::milliseconds(200));
        }
        path = "characters/cthulhu_scout_back.png";
        m_animCthulhuScout = gf::Animation{};
        for(std::size_t i = 0; i < 5; ++i){
            gf::RectF bounds{i * 256.0f,0.0f,256.0f,256.0f};
            m_animCthulhuScout.addFrame(m_resMgr->getTexture(path), bounds, gf::milliseconds(200));
        }
        path = "characters/satan_scout.png";
        m_animSatanScout = gf::Animation{};
        for(std::size_t i = 0; i < 6; ++i){
            gf::RectF bounds{i * 256.0f,0.0f,256.0f,256.0f};
            m_animSatanScout.addFrame(m_resMgr->getTexture(path), bounds, gf::milliseconds(200));
        }
    }

    const Gameboard* m_board;

    gf::ResourceManager* m_resMgr;
    gf::EntityContainer* m_entityMgr;

    gf::Sprite m_darkTile{m_resMgr->getTexture("case.png")};
    gf::Sprite m_brightTile{m_resMgr->getTexture("case2.png")};
    gf::Animation m_animCthulhuTank;
    gf::Animation m_animSatanTank;
    gf::Animation m_animCthulhuSupport;
    gf::Animation m_animSatanSupport;
    gf::Animation m_animCthulhuScout;
    gf::Animation m_animSatanScout;

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

    gf::Sprite m_magicLock{m_resMgr->getTexture("locked.png")};

    std::map<gf::Vector2i, std::unique_ptr<EntityCharacter>, PositionComp> m_entities{};
};

#endif //CTHULHUVSSATAN_GAMEBOARDVIEW_H
