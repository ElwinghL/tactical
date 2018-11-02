#include "game.h"

#include <iostream>

Game::Game(gf::ResourceManager* resMgr) :
    m_resMgr{resMgr}
{
    initWindow();
    initViews();
    initActions();
    initWidgets();
    initEntities();
}

void Game::processEvents()
{
    gf::Event event{};

    while (m_window.pollEvent(event)) {
        if (event.type == gf::EventType::MouseMoved) {
            m_mouseCoords = m_renderer.mapPixelToCoords(event.mouseCursor.coords);
        }

        m_actions.processEvent(event);
        m_views.processEvent(event);
    }

    if (m_closeWindowAction.isActive()) {
        m_window.close();
    }

    if (m_fullscreenAction.isActive()) {
        m_window.toggleFullscreen();
    }

    switch (m_gameState) {
    case GameState::MainMenu: {
        if (m_leftClickAction.isActive()) {
            m_menuWidgets.pointTo(m_mouseCoords);
            if (m_playButton.contains(m_mouseCoords) || m_quitButton.contains(m_mouseCoords)) {
                m_menuWidgets.triggerAction();
            }
        }
    } break;

    case GameState::Pause: {
    } break;

    case GameState::GameStart: {
    } break;

    case GameState::PlayerTurn: {
    } break;

    case GameState::WaitingForAI: {
    } break;

    case GameState::GameEnd: {
    } break;
    }

    m_actions.reset();
}

void Game::update()
{
    gf::Time time = m_clock.restart();
    switch (m_gameState) {
    case GameState::MainMenu: {
    } break;

    case GameState::Pause: {
    } break;

    case GameState::GameStart: {
        for (int y = 0; y < m_board.getRows(); ++y) {
            for (int x = 0; x < m_board.getCols(); ++x) {
                if (m_board(gf::Vector2i{x, y})) {
                    switch (m_board(gf::Vector2i{x, y})->getType()) {
                    case CharacterType::Tank:
                        std::cout << "T ";
                        break;
                    case CharacterType::Scout:
                        std::cout << "s ";
                        break;
                    case CharacterType::Support:
                        std::cout << "S ";
                    }
                } else {
                    std::cout << "  ";
                }
            }

            std::cout << std::endl;
        }

        m_clearColor = gf::Color::Black;

        m_gameState = GameState::PlayerTurn;
    } break;

    case GameState::PlayerTurn: {
        m_mainEntities.update(time);
    } break;

    case GameState::WaitingForAI: {
        m_mainEntities.update(time);
    } break;

    case GameState::GameEnd: {
    } break;
    }
}

void Game::render()
{
    m_renderer.clear(m_clearColor);

    switch (m_gameState) {
    case GameState::MainMenu: {
        m_renderer.setView(m_menuView);
        m_renderer.draw(m_title);
        m_menuWidgets.render(m_renderer);
    } break;

    case GameState::Pause: {
        m_renderer.setView(m_mainView);
        m_mainEntities.render(m_renderer);
    } break;

    case GameState::GameStart: {
    } break;

    case GameState::PlayerTurn: {
        m_renderer.setView(m_mainView);
        m_mainEntities.render(m_renderer);
    } break;

    case GameState::WaitingForAI: {
        m_renderer.setView(m_mainView);
        m_mainEntities.render(m_renderer);
    } break;

    case GameState::GameEnd: {
    } break;
    }

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
    m_views.addView(m_menuView);

    m_views.setInitialScreenSize(m_screenSize);
}

void Game::initActions()
{
    m_closeWindowAction.addCloseControl();
    m_actions.addAction(m_closeWindowAction);

    m_fullscreenAction.addKeycodeKeyControl(gf::Keycode::F);
    m_actions.addAction(m_fullscreenAction);

    m_leftClickAction.addMouseButtonControl(gf::MouseButton::Left);
    m_actions.addAction(m_leftClickAction);
}

void Game::initWidgets()
{
    m_title.setAnchor(gf::Anchor::BottomCenter);
    m_title.setColor(gf::Color::Black);
    m_title.setPosition(gf::Vector2f{0.0f, -100.0f});

    auto buttonInit = [this](gf::TextButtonWidget& button, gf::Anchor anchor, const gf::Vector2f& posToCenter) {
        button.setAnchor(anchor);
        button.setPosition(posToCenter);

        button.setDefaultBackgroundColor(gf::Color::Black);
        button.setDefaultTextColor(gf::Color::White);

        button.setPadding(10.0f);
        button.setRadius(5.0f);

        button.setState(gf::WidgetState::Default);

        m_menuWidgets.addWidget(button);
    };

    buttonInit(m_playButton, gf::Anchor::BottomCenter, {0.0f, -20.0f});
    m_playButton.setCallback([this] {
        m_gameState = GameState::GameStart;
    });

    buttonInit(m_quitButton, gf::Anchor::TopCenter, {0.0f, 20.0f});
    m_quitButton.setCallback([this] {
        m_window.close();
    });
}

void Game::initEntities()
{
    m_humanPlayer.setGoalPositions({gf::Vector2i{1, 1}, gf::Vector2i{1, 4}});
    m_aiPlayer.setGoalPositions({gf::Vector2i{10, 1}, gf::Vector2i{10, 4}});

    auto initPlayerCharacters{[this](int column, Player& player) {
        gf::Vector2i pos{column, 0};

        auto addCharacterInColumn{[this, &pos, &player](CharacterType type) {
            addCharacter(player, Character{player.getTeam(), type, pos});
            ++pos.y;
        }};

        addCharacterInColumn(CharacterType::Scout);
        addCharacterInColumn(CharacterType::Support);
        addCharacterInColumn(CharacterType::Tank);
        addCharacterInColumn(CharacterType::Tank);
        addCharacterInColumn(CharacterType::Support);
        addCharacterInColumn(CharacterType::Scout);
    }};

    initPlayerCharacters(2, m_humanPlayer);
    initPlayerCharacters(9, m_aiPlayer);
}
