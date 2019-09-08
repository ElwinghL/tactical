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
#include <map>

#include <memory>
#include <cassert>

class Action;

class Goal;

class GameboardView {
public:
    explicit GameboardView(const Gameboard& board, gf::ResourceManager& resMgr, gf::EntityContainer& entityMgr);

    [[nodiscard]] bool animationFinished() const;

    void drawGrid(gf::RenderTarget& target, const gf::RenderStates& states = gf::RenderStates{});

    void update();

    void notifyMove(const gf::Vector2i& origin, const gf::Vector2i& dest);

    void notifyHP(const gf::Vector2i& pos, int hp);

private:
    class EntityCharacter : public gf::Entity {
    public:
        inline explicit EntityCharacter(GameboardView& gbView, gf::Sprite sprite, int hp, const gf::Vector2i& pos);

        void update(gf::Time time) override;

        void render(gf::RenderTarget& target, const gf::RenderStates& states) override;

        inline void setHP(int hp);

        void moveTo(const gf::Vector2i& pos);

        inline void setLocked(bool locked);

        [[nodiscard]] inline bool animationFinished() const;

    private:
        [[nodiscard]] static inline std::size_t checkHP(int hp);

        void drawLife(gf::RenderTarget& target, const gf::RenderStates& states);

        GameboardView* m_gbView;
        gf::Sprite m_sprite;

        gf::Vector2f m_origin;
        gf::Vector2f m_dest;

        float m_timeFrac;

        std::size_t m_currentHPSprite;

        bool m_locked{false};
    };

    [[nodiscard]] constexpr std::size_t getActivationIndex(bool activated) const;

    const Gameboard* m_board;

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

    gf::Sprite m_magicLock{m_resMgr->getTexture("locked.png")};

    std::map<gf::Vector2i, std::unique_ptr<EntityCharacter>, PositionComp> m_entities{};
};

#include "impl/gameboardview.h"

#endif //CTHULHUVSSATAN_GAMEBOARDVIEW_H
