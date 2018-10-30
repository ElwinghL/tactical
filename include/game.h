#ifndef GAME_H
#define GAME_H

#include <gf/Action.h>
#include <gf/Clock.h>
#include <gf/Color.h>
#include <gf/EntityContainer.h>
#include <gf/Event.h>
#include <gf/Font.h>
#include <gf/RenderWindow.h>
#include <gf/ResourceManager.h>
#include <gf/Text.h>
#include <gf/ViewContainer.h>
#include <gf/Views.h>
#include <gf/WidgetContainer.h>
#include <gf/Widgets.h>
#include <gf/Window.h>

class Game {
public:
    Game(gf::ResourceManager* resMgr);

    bool isRunning();

    void processEvents();
    void update();
    void render();

private:
    enum class GameState {
        MainMenu,
        Pause,
        GameStart,
        PlayerTurn,
        WaitingForAI,
        GameEnd
    };

    void initWindow();
    void initViews();
    void initActions();
    void initWidgets();
    void initEntities();

    const gf::Vector2u m_screenSize{1024, 576};
    const gf::Vector2f m_viewSize{100.0f, 100.0f};
    const gf::Vector2f m_viewCenter{0.0f, 0.0f};

    gf::Vector4f m_clearColor{gf::Color::White};

    gf::Vector2f m_mouseCoords{};

    GameState m_gameState{GameState::MainMenu};

    gf::ResourceManager* m_resMgr{};

    gf::Window m_window{"Cthulhu vs Satan", m_screenSize};
    gf::RenderWindow m_renderer{m_window};

    gf::ViewContainer m_views{};
    gf::ExtendView m_mainView{m_viewCenter, m_viewSize};
    gf::ExtendView m_menuView{m_viewCenter, gf::Vector2f{800.0f, 450.0f}};

    gf::ActionContainer m_actions{};
    gf::Action m_closeWindowAction{"Close window"};
    gf::Action m_fullscreenAction{"Fullscreen"};
    gf::Action m_leftClickAction{"Left click"};

    gf::Text m_title{"Cthulhu vs Satan", m_resMgr->getFont("fonts/title.ttf")};

    gf::WidgetContainer m_menuWidgets{};

    gf::Font& m_buttonFont{m_resMgr->getFont("fonts/button.ttf")};

    gf::TextButtonWidget m_playButton{"Play!", m_buttonFont};
    gf::TextButtonWidget m_quitButton{"Quit", m_buttonFont};

    gf::EntityContainer m_mainEntities{};

    gf::Clock m_clock{};
};

#endif // GAME_H
