/**
 * The file defining the game and how it runs
 * \author Fabien Matusalem
 */
#ifndef GAME_H
#define GAME_H

#include "entitycharacter.h"
#include "gameai.h"
#include "humanplayer.h"
#include "player.h"

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

#include <string>
#include <utility>
#include <vector>
#include <stdlib.h>
#include <time.h>

/**
 * The game core class
 */
class Game {
public:
    /**
     * Constructor
     * \param resMgr The resource manager of the game
     */
    explicit Game(gf::ResourceManager* resMgr);

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

    /**
     * Give the size of the game board
     * \return The width and the height of the game board
     */
    gf::Vector2i getBoardSize() const
    {
        return m_board.getSize();
    }

    /**
     * Tell if a position is valid
     * \param pos The position to test
     * \return True if the position is inside the game board
     */
    bool positionIsValid(const gf::Vector2i& pos) const
    {
        return m_board.isValid(pos);
    }

private:
    enum class GameState {
        MainMenu,
        Pause,
        GameStart,
        PlayerTurn,
        WaitingForAI,
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
    void initEntities();

    void addCharacter(Player& player, Character&& character);

    /**
     * Get a pointer to the character at this position, nullptr if the player
     * has no character at this position
     * \param pos The specified position
     * \param team The team of the player
     * \return The pointer to the character, or nullptr
     */
    Character* getCharacter(gf::Vector2i pos, PlayerTeam team);

    /**
     * Get a pointer to the character at this position, nullptr if there is no character
     * \param pos The specified position
     * \return The pointer to the character, or nullptr
     */
    Character* getCharacter(gf::Vector2i pos);

    /**
     * Moves the character to the position
     * \param character a pointer to the character to move
     * \param pos the position where to move the character
     * \return true if the character was successly moved
     */
    bool moveCharacter(Character* character, gf::Vector2i pos);

    void stateSelectionUpdate(PlayerTurnSelection nextState);

    /**
     * Draw the background of the game
     */
    void drawBackground();

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

    gf::Vector4f m_clearColor{gf::Color::White};

    gf::Vector2f m_mouseCoords{};

    GameState m_gameState{GameState::MainMenu};

    PlayerTurnSelection m_playerTurnSelection{PlayerTurnSelection::NoSelection};

    gf::ResourceManager* m_resMgr{nullptr};

    gf::Window m_window{getName(), m_screenSize};
    gf::RenderWindow m_renderer{m_window};

    gf::ViewContainer m_views{};
    gf::ExtendView m_mainView{m_viewCenter, m_viewSize};
    gf::ExtendView m_menuView{m_viewCenter, gf::Vector2f{800.0f, 450.0f}};

    gf::ActionContainer m_actions{};
    gf::Action m_closeWindowAction{"Close window"};
    gf::Action m_fullscreenAction{"Fullscreen"};
    gf::Action m_leftClickAction{"Left click"};

    gf::Text m_title{getName(), m_resMgr->getFont("fonts/title.ttf")};

    gf::WidgetContainer m_menuWidgets{};

    gf::Font& m_buttonFont{m_resMgr->getFont("fonts/button.ttf")};

    gf::TextButtonWidget m_playButton{"Play!", m_buttonFont};
    gf::TextButtonWidget m_quitButton{"Quit", m_buttonFont};

    std::vector<EntityCharacter> m_characterEntities{};
    //    gf::EntityContainer m_mainEntities{};

    gf::Clock m_clock{};

    HumanPlayer m_humanPlayer{PlayerTeam::Cthulhu};
    GameAI m_aiPlayer{PlayerTeam::Satan};

    Character* m_selectedCharacter = NULL;

    gf::Array2D<Character*, int> m_board{{12, 6}, nullptr};

    std::set<gf::Vector2i, PositionComp> m_possibleTargets;
    std::set<gf::Vector2i, PositionComp> m_targetsInRange;

    gf::Sprite m_darkTile{m_resMgr->getTexture("placeholders/case.png")};
    gf::Sprite m_brightTile{m_resMgr->getTexture("placeholders/case2.png")};
    gf::Sprite m_selectedTile{m_resMgr->getTexture("placeholders/caseSelected.png")};
    gf::Sprite m_possibleTargetsTile{m_resMgr->getTexture("placeholders/casePossibleTargets.png")};
    gf::Sprite m_targetsInRangeTile{m_resMgr->getTexture("placeholders/caseTargetsInRange.png")};

    gf::SpriteWidget m_buttonAttack{m_resMgr->getTexture("placeholders/iconAttack.png"), m_resMgr->getTexture("placeholders/iconAttack.png"), m_resMgr->getTexture("placeholders/iconAttack.png")};
    gf::SpriteWidget m_buttonCapacity{m_resMgr->getTexture("placeholders/iconCapacity.png"), m_resMgr->getTexture("placeholders/iconCapacity.png"), m_resMgr->getTexture("placeholders/iconCapacity.png")};

    gf::WidgetContainer m_uiWidgets{};
};

#endif // GAME_H
