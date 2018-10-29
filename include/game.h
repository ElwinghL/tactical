#ifndef GAME_H
#define GAME_H

#include <gf/Action.h>
#include <gf/Clock.h>
#include <gf/Color.h>
#include <gf/EntityContainer.h>
#include <gf/Event.h>
#include <gf/RenderWindow.h>
#include <gf/ViewContainer.h>
#include <gf/Views.h>
#include <gf/Window.h>

class Game {
public:
    Game();

    bool isRunning();

    void processEvents();
    void update();
    void render();

private:
    void initWindow();
    void initViews();
    void initActions();
    void initEntities();

    const gf::Vector2u m_screenSize {1024, 576};
    const gf::Vector2f m_viewSize {100.0f, 100.0f};
    const gf::Vector2f m_viewCenter {0.0f, 0.0f};

    const gf::Vector4f m_clearColor {gf::Color::Black};

    gf::Window m_window {"Cthulhu vs Satan", m_screenSize};
    gf::RenderWindow m_renderer {m_window};

    gf::ViewContainer m_views {};
    gf::ExtendView m_mainView {m_viewCenter, m_viewSize};
    gf::ScreenView m_hudView {};

    gf::ActionContainer m_actions {};
    gf::Action m_closeWindowAction {"Close window"};
    gf::Action m_fullscreenAction {"Fullscreen"};

    gf::EntityContainer m_mainEntities {};
    gf::EntityContainer m_hudEntities {};

    gf::Clock m_clock {};
};

#endif // GAME_H
