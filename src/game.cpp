#include "game.h"

Game::Game()
{
    initWindow();
    initViews();
    initActions();
    initEntities();

    m_renderer.clear(m_clearColor);
}

bool Game::isRunning()
{
    return m_window.isOpen();
}

void Game::processEvents()
{
    gf::Event event;

    while (m_window.pollEvent(event)) {
        m_actions.processEvent(event);
        m_views.processEvent(event);
    }

    if (m_closeWindowAction.isActive()) {
        m_window.close();
    }

    if (m_fullscreenAction.isActive()) {
        m_window.toggleFullscreen();
    }

    m_actions.reset();
}

void Game::update()
{
    gf::Time time = m_clock.restart();
    m_mainEntities.update(time);
    m_hudEntities.update(time);
}

void Game::render()
{
    m_renderer.clear();

    m_renderer.setView(m_mainView);
    m_mainEntities.render(m_renderer);

    m_renderer.setView(m_hudView);
    m_hudEntities.render(m_renderer);

    m_renderer.display();
}

void Game::initWindow()
{
    m_window.setVerticalSyncEnabled(true);
    m_window.setFramerateLimit(60);
}

void Game::initViews()
{
    m_views.addView(m_mainView);
    m_views.addView(m_hudView);

    m_views.setInitialScreenSize(m_screenSize);
}

void Game::initActions()
{
    m_closeWindowAction.addCloseControl();
    m_closeWindowAction.addKeycodeKeyControl(gf::Keycode::Escape);
    m_actions.addAction(m_closeWindowAction);

    m_fullscreenAction.addKeycodeKeyControl(gf::Keycode::F);
    m_actions.addAction(m_fullscreenAction);
}

void Game::initEntities()
{
    // TODO
}
