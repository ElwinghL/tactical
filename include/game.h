/**
 * The file defining the game and how it runs
 * \author Fabien Matusalem
 */
#ifndef GAME_H
#define GAME_H

#include "gameai.h"
#include "gameboard.h"
#include "humanplayer.h"
#include "player.h"
#include "gameboardview.h"

#include <gf/Action.h>
#include <gf/Array2D.h>
#include <gf/Clock.h>
#include <gf/Color.h>
#include <gf/EntityContainer.h>
#include <gf/Event.h>
#include <gf/Font.h>
#include <gf/RenderWindow.h>
#include <gf/ResourceManager.h>
#include <gf/Sprite.h>
#include <gf/Text.h>
#include <gf/Vector.h>
#include <gf/ViewContainer.h>
#include <gf/Views.h>
#include <gf/WidgetContainer.h>
#include <gf/Widgets.h>
#include <gf/Window.h>

#include <boost/optional.hpp>
#include <string>
#include <utility>
#include <vector>

/**
 * The game core class
 */
class Game {
public:
    /**
     * Constructor
     * \param resMgr The resource manager of the game
     */
    explicit Game(gf::ResourceManager& resMgr);

    /**
     * Tell if the game is running
     * \return True if the game is running, false if it has closed
     */
    bool isRunning()
    {
        return m_window.isOpen();
    }

    /**
     * Update the input states
     */
    void processEvents();

    /**
     * Update the game logic according to the inputs
     */
    void update();

    /**
     * Display the game on the screen
     */
    void render();

    /**
     * Give the name of the game
     * \return The game's name, "Cthulhu vs Satan"
     */
    std::string getName() const
    {
        return "Cthulhu vs Satan";
    }

private:
    enum class GameState {
        MainMenu,
        Playing,
        GameEnd
    };

    enum class PlayerTurnSelection {
        NoSelection,
        MoveSelection,
        AttackSelection,
        CapacitySelection
    };

    void initWindow();
    void initViews();
    void initActions();
    void initWidgets();
    void initSprites();

    bool isFromTeam(const gf::Vector2i& tile, PlayerTeam team) const
    {
        return m_board.isOccupied(tile) && m_board.getCharacter(tile).getTeam() == team;
    }

    void stateSelectionUpdate(PlayerTurnSelection nextState);

    /**
     * Draw targets for the selected character
     */
    void drawTargets();

    /**
     * Draw the user interface of the game
     */
    void drawUI();

    /**
     * Switch turn
     */
    void switchTurn();

    const gf::Vector2u m_screenSize{1024, 576};
    const gf::Vector2f m_viewSize{100.0f, 100.0f};
    const gf::Vector2f m_viewCenter{0.0f, 0.0f};

    gf::Color4f m_clearColor{gf::Color::White};

    gf::Vector2f m_mouseCoords{};

    GameState m_gameState{GameState::MainMenu};

    PlayerTurnSelection m_playerTurnSelection{PlayerTurnSelection::NoSelection};

    gf::ResourceManager* m_resMgr{nullptr};

    gf::Window m_window{getName(), m_screenSize};
    gf::RenderWindow m_renderer{m_window};

    gf::ViewContainer m_views{};
    gf::ExtendView m_menuView{m_viewCenter, gf::Vector2f{800.0f, 450.0f}};
    gf::FillView m_backMenuView{m_viewCenter, gf::Vector2f{1920.0f, 1080.0f}};
    gf::ExtendView m_mainView{m_viewCenter, m_viewSize};

    gf::ActionContainer m_actions{};
    gf::Action m_closeWindowAction{"Close window"};
    gf::Action m_fullscreenAction{"Fullscreen"};
    gf::Action m_leftClickAction{"Left click"};

    gf::Text m_winText{"Vous avez invoqué votre divinité !", m_resMgr->getFont("title.ttf")};
    gf::Text m_defeatText{"L'adversaire vous a écrasé avec sa divinité", m_resMgr->getFont("title.ttf")};

    gf::WidgetContainer m_menuWidgets{};

    gf::Font& m_buttonFont{m_resMgr->getFont("button.ttf")};

    gf::TextButtonWidget m_playButton{"Jouer !", m_buttonFont};
    gf::TextButtonWidget m_quitButton{"Quitter", m_buttonFont};

    gf::EntityContainer m_entityMgr{};

    gf::Clock m_clock{};

    HumanPlayer m_humanPlayer{PlayerTeam::Cthulhu};
    GameAI m_aiPlayer{PlayerTeam::Satan};

    boost::optional<gf::Vector2i> m_selectedPos;

    std::unique_ptr<GameboardView> m_gbView{nullptr}; // Deleted after gameboard because of callbacks
    Gameboard m_board{};

    std::set<gf::Vector2i, PositionComp> m_possibleTargets;
    std::set<gf::Vector2i, PositionComp> m_targetsInRange;

    gf::Sprite m_gameBackground{m_resMgr->getTexture("decor/gameBackground.png")};

    gf::Sprite m_selectedTile{m_resMgr->getTexture("caseSelected.png")};
    gf::Sprite m_possibleTargetsTile{m_resMgr->getTexture("casePossibleTargets.png")};
    gf::Sprite m_targetsInRangeTile{m_resMgr->getTexture("caseTargetsInRange.png")};

    gf::Sprite m_infoboxScout{m_resMgr->getTexture("infoboxScout.png")};
    gf::Sprite m_infoboxTank{m_resMgr->getTexture("infoboxTank.png")};
    gf::Sprite m_infoboxSupport{m_resMgr->getTexture("infoboxSupport.png")};
    gf::Sprite m_infoboxScoutAttack{m_resMgr->getTexture("infoboxScoutAttack.png")};
    gf::Sprite m_infoboxTankAttack{m_resMgr->getTexture("infoboxTankAttack.png")};
    gf::Sprite m_infoboxSupportAttack{m_resMgr->getTexture("infoboxSupportAttack.png")};
    gf::Sprite m_infoboxScoutCapacity{m_resMgr->getTexture("infoboxScoutCapacity.png")};
    gf::Sprite m_infoboxTankCapacity{m_resMgr->getTexture("infoboxTankCapacity.png")};
    gf::Sprite m_infoboxSupportCapacity{m_resMgr->getTexture("infoboxSupportCapacity.png")};
    gf::Sprite m_infoboxPass{m_resMgr->getTexture("infoboxPass.png")};

    gf::Sprite m_menuBackground{m_resMgr->getTexture("menu/background.png")};
    gf::Sprite m_title{m_resMgr->getTexture("menu/titre.png")};

    gf::SpriteWidget m_buttonAttack{m_resMgr->getTexture("UI/buttonAttack3.png"),
                                    m_resMgr->getTexture("UI/buttonAttack1.png"),
                                    m_resMgr->getTexture("UI/buttonAttack2.png")};
    gf::SpriteWidget m_buttonCapacity{m_resMgr->getTexture("UI/buttonCapacity3.png"),
                                      m_resMgr->getTexture("UI/buttonCapacity1.png"),
                                      m_resMgr->getTexture("UI/buttonCapacity2.png")};
    gf::SpriteWidget m_buttonPass{m_resMgr->getTexture("UI/buttonPass3.png"),
                                  m_resMgr->getTexture("UI/buttonPass1.png"),
                                  m_resMgr->getTexture("UI/buttonPass2.png")};

    gf::WidgetContainer m_uiWidgets{};
};

#endif // GAME_H
