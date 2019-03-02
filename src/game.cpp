#include "game.h"

#include <gf/SpriteBatch.h>

#include <functional>

Game::Game(gf::ResourceManager& resMgr) :
    m_resMgr{&resMgr}
{
    using namespace std::placeholders;

    m_gbView = std::make_unique<GameboardView>(m_board, *m_resMgr, m_entityMgr);

    m_board.setMoveCallback(std::bind(&GameboardView::notifyMove, m_gbView.get(), _1, _2));
    m_board.setHPChangeCallback(std::bind(&GameboardView::notifyHP, m_gbView.get(), _1, _2));

    initWindow();
    initViews();
    initActions();
    initWidgets();
    initSprites();

    m_aiPlayer.setInitialGameboard(m_board);
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
            m_menuWidgets.triggerAction();
        }
    } break;

    case GameState::Playing: {
        if (!m_gbView->animationFinished()) {
            break;
        }

        if (m_board.isCallbackNeeded()) {
            m_board.doFirstCallback();
            break;
        }

        if (m_board.getPlayingTeam() == m_humanPlayer.getTeam()) {
            if (m_leftClickAction.isActive()) {
                gf::Vector2i tile{screenToGamePos(m_mouseCoords)};


                if (m_selectedPos) {
                    if (m_buttonAttack.contains(m_mouseCoords)) {
                        stateSelectionUpdate(PlayerTurnSelection::AttackSelection);
                        break;
                    }

                    if (m_buttonCapacity.contains(m_mouseCoords)) {
                        stateSelectionUpdate(PlayerTurnSelection::CapacitySelection);
                        break;
                    }

                    if (m_buttonPass.contains(m_mouseCoords)) {
                        switchTurn();
                        break;
                    }
                }

                switch (m_playerTurnSelection) {
                case PlayerTurnSelection::NoSelection: {
                    if (isFromTeam(tile, m_humanPlayer.getTeam())) {
                        m_selectedPos = tile;
                        stateSelectionUpdate(PlayerTurnSelection::MoveSelection);
                    } else {
                        m_selectedPos = boost::none;
                        stateSelectionUpdate(PlayerTurnSelection::NoSelection);
                    }
                } break;

                case PlayerTurnSelection::MoveSelection: {
                    assert(m_selectedPos);

                    if (m_selectedPos != tile) {
                        if (isFromTeam(tile, m_humanPlayer.getTeam())) {
                            m_selectedPos = tile;
                            stateSelectionUpdate(PlayerTurnSelection::MoveSelection);
                        } else if (m_board.move(*m_selectedPos, tile)) {
                            m_humanPlayer.setMoved(true);
                            m_selectedPos = tile;
                            stateSelectionUpdate(PlayerTurnSelection::AttackSelection);
                        } else {
                            m_selectedPos = boost::none;
                            stateSelectionUpdate(PlayerTurnSelection::NoSelection);
                        }
                    } else {
                        m_selectedPos = boost::none;
                        stateSelectionUpdate(PlayerTurnSelection::NoSelection);
                    }
                } break;

                case PlayerTurnSelection::AttackSelection: {
                    assert(m_selectedPos);

                    if (m_board.attack(*m_selectedPos, tile)) {
                        m_selectedPos = tile;
                        switchTurn();
                    } else if (!m_humanPlayer.hasMoved()) {
                        m_selectedPos = boost::none;
                        stateSelectionUpdate(PlayerTurnSelection::NoSelection);
                    }
                } break;

                case PlayerTurnSelection::CapacitySelection: {
                    assert(m_selectedPos);

                    if (m_board.useCapacity(*m_selectedPos, tile)) {
                        m_selectedPos = tile;
                        switchTurn();
                    } else if (!m_humanPlayer.hasMoved()) {
                        m_selectedPos = boost::none;
                        stateSelectionUpdate(PlayerTurnSelection::NoSelection);
                    }
                } break;
                }
            }
        } else if (m_aiPlayer.playTurn(m_board)) {
            switchTurn();
        }
    } break;

    case GameState::GameEnd: {
        if (m_board.isCallbackNeeded()) {
            m_board.doFirstCallback();
            break;
        }
    } break;
    }

    m_actions.reset();
}

void Game::stateSelectionUpdate(PlayerTurnSelection nextState)
{
    if (nextState == PlayerTurnSelection::NoSelection) {
        m_playerTurnSelection = nextState;
        return;
    }

    if (!m_selectedPos) {
        return;
    }

    //Mise à jour des cases à surligner visuellement :
    m_possibleTargets.clear();
    m_targetsInRange.clear();
    switch (nextState) {
    case PlayerTurnSelection::MoveSelection: {
        m_possibleTargets = m_board.getAllPossibleMoves(*m_selectedPos);
        m_targetsInRange = m_board.getAllPossibleMoves(*m_selectedPos, true);
    } break;
    case PlayerTurnSelection::AttackSelection: {
        m_possibleTargets = m_board.getAllPossibleAttacks(*m_selectedPos);
        m_targetsInRange = m_board.getAllPossibleAttacks(*m_selectedPos, true);
    } break;
    case PlayerTurnSelection::CapacitySelection: {
        m_possibleTargets = m_board.getAllPossibleCapacities(*m_selectedPos);
        m_targetsInRange = m_board.getAllPossibleCapacities(*m_selectedPos, true);
    } break;
        // TODO Refactor or add something

    case PlayerTurnSelection::NoSelection:
        break;
    }
    m_playerTurnSelection = nextState;
}

void Game::update()
{
    gf::Time time = m_clock.restart();
    switch (m_gameState) {
    case GameState::MainMenu: {
    } break;

    case GameState::Playing: {
        m_entityMgr.update(time);
        m_gbView->update();
        if (m_board.hasWon(PlayerTeam::Cthulhu) || m_board.hasWon(PlayerTeam::Satan)) {
            m_gameState = GameState::GameEnd;
        }
    } break;

    case GameState::GameEnd: {
        m_entityMgr.update(time);
        m_gbView->update();
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

    case GameState::Playing:
    case GameState::GameEnd: {
        m_renderer.setView(m_mainView);
        m_gbView->drawGrid(m_renderer);
        bool animationFinished = m_gbView->animationFinished();
        if (animationFinished && m_gameState == GameState::Playing) {
            drawTargets();
        }
        m_entityMgr.render(m_renderer);
        if (animationFinished) {
            drawUI();
        }
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
    resizeView(m_mainView, m_board.getSize());

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

    m_defeatText.setAnchor(gf::Anchor::Center);
    m_defeatText.setOutlineThickness(1.0f);
    m_defeatText.setOutlineColor(gf::Color::Black);
    m_defeatText.setColor(gf::Color::Red);
    m_defeatText.setPosition(gf::Vector2f{0.0f, 0.0f});

    m_winText.setAnchor(gf::Anchor::Center);
    m_winText.setOutlineThickness(1.0f);
    m_winText.setOutlineColor(gf::Color::Black);
    m_winText.setColor(gf::Color::Green);
    m_winText.setPosition(gf::Vector2f{0.0f, 0.0f});

    m_uiWidgets.addWidget(m_buttonAttack);
    m_uiWidgets.addWidget(m_buttonCapacity);
    m_uiWidgets.addWidget(m_buttonPass);

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
        m_clearColor = gf::Color::Black;
        m_gameState = GameState::Playing;
    });

    buttonInit(m_quitButton, gf::Anchor::TopCenter, {0.0f, 20.0f});
    m_quitButton.setCallback([this] {
        m_window.close();
    });
}

void Game::initSprites()
{
    m_selectedTile.setAnchor(gf::Anchor::Center);
    m_possibleTargetsTile.setAnchor(gf::Anchor::Center);
    m_targetsInRangeTile.setAnchor(gf::Anchor::Center);

    m_buttonAttack.setAnchor(gf::Anchor::TopLeft);
    m_buttonCapacity.setAnchor(gf::Anchor::TopLeft);
    m_buttonPass.setAnchor(gf::Anchor::TopLeft);

    gf::Vector2f infoboxScale{0.3f, 0.3f};
    m_infoboxScout.setAnchor(gf::Anchor::TopLeft);
    m_infoboxScout.setScale(infoboxScale);
    m_infoboxTank.setAnchor(gf::Anchor::TopLeft);
    m_infoboxTank.setScale(infoboxScale);
    m_infoboxSupport.setAnchor(gf::Anchor::TopLeft);
    m_infoboxSupport.setScale(infoboxScale);
    m_infoboxScoutAttack.setAnchor(gf::Anchor::TopRight);
    m_infoboxScoutAttack.setScale(infoboxScale);
    m_infoboxTankAttack.setAnchor(gf::Anchor::TopRight);
    m_infoboxTankAttack.setScale(infoboxScale);
    m_infoboxSupportAttack.setAnchor(gf::Anchor::TopRight);
    m_infoboxSupportAttack.setScale(infoboxScale);
    m_infoboxScoutCapacity.setAnchor(gf::Anchor::TopRight);
    m_infoboxScoutCapacity.setScale(infoboxScale);
    m_infoboxTankCapacity.setAnchor(gf::Anchor::TopRight);
    m_infoboxTankCapacity.setScale(infoboxScale);
    m_infoboxSupportCapacity.setAnchor(gf::Anchor::TopRight);
    m_infoboxSupportCapacity.setScale(infoboxScale);
    m_infoboxPass.setAnchor(gf::Anchor::TopRight);
    m_infoboxPass.setScale(infoboxScale);
}

void Game::drawUI()
{
    if (m_gameState == GameState::Playing) {
        if (m_selectedPos) {
            //Dans l'idée, ça serait plus ergonomique d'afficher les boutons en bas à droite
            gf::Vector2i posButtonAttack{465, 15};
            m_buttonAttack.setPosition(posButtonAttack);
            gf::Vector2i posButtonCapacity{500, 15};
            m_buttonCapacity.setPosition(posButtonCapacity);
            gf::Vector2i posButtonPass{535, 15};
            m_buttonPass.setPosition(posButtonPass);
            m_uiWidgets.render(m_renderer);
        }

        //Affichage des infobox :
        gf::Vector2i tile{screenToGamePos(m_mouseCoords)};
        gf::SpriteBatch batch{m_renderer};
        batch.begin();
        if (m_board.isOccupied(tile)) {
            CharacterType mouseOverType = m_board.getTypeFor(tile);
            switch (mouseOverType) {
            case CharacterType::Scout: {
                m_infoboxScout.setPosition(m_mouseCoords);
                batch.draw(m_infoboxScout);
                break;
            }
            case CharacterType::Tank: {
                m_infoboxTank.setPosition(m_mouseCoords);
                batch.draw(m_infoboxTank);
                break;
            }
            case CharacterType::Support: {
                m_infoboxSupport.setPosition(m_mouseCoords);
                batch.draw(m_infoboxSupport);
                break;
            }
            }
        }
        if (m_buttonAttack.contains(m_mouseCoords) && m_selectedPos) {
            assert(m_board.isOccupied(*m_selectedPos));
            switch (m_board.getTypeFor(*m_selectedPos)) {
            case CharacterType::Scout: {
                m_infoboxScoutAttack.setPosition(m_mouseCoords);
                batch.draw(m_infoboxScoutAttack);
                break;
            }
            case CharacterType::Tank: {
                m_infoboxTankAttack.setPosition(m_mouseCoords);
                batch.draw(m_infoboxTankAttack);
                break;
            }
            case CharacterType::Support: {
                m_infoboxSupportAttack.setPosition(m_mouseCoords);
                batch.draw(m_infoboxSupportAttack);
                break;
            }
            }
        }
        if (m_buttonCapacity.contains(m_mouseCoords) && m_selectedPos) {
            assert(m_board.isOccupied(*m_selectedPos));
            switch (m_board.getTypeFor(*m_selectedPos)) {
            case CharacterType::Scout: {
                m_infoboxScoutCapacity.setPosition(m_mouseCoords);
                batch.draw(m_infoboxScoutCapacity);
                break;
            }
            case CharacterType::Tank: {
                m_infoboxTankCapacity.setPosition(m_mouseCoords);
                batch.draw(m_infoboxTankCapacity);
                break;
            }
            case CharacterType::Support: {
                m_infoboxSupportCapacity.setPosition(m_mouseCoords);
                batch.draw(m_infoboxSupportCapacity);
                break;
            }
            }
        }
        if (m_buttonPass.contains(m_mouseCoords) && m_selectedPos) {
            m_infoboxPass.setPosition(m_mouseCoords);
            batch.draw(m_infoboxPass);
        }
        batch.end();
    } else {
        m_renderer.setView(m_menuView);
        if (m_board.hasWon(m_humanPlayer.getTeam())) {
            m_renderer.draw(m_winText);
        } else {
            m_renderer.draw(m_defeatText);
        }
    }
}

void Game::drawTargets()
{
    gf::SpriteBatch batch{m_renderer};
    batch.begin();
    m_board.forEach([this, &batch](auto pos) {
        bool tileSelected = m_selectedPos && *m_selectedPos == pos;
        bool showPossibleTargets = m_selectedPos && m_possibleTargets.count(pos) > 0;
        bool showTargetsInRange = m_selectedPos && m_targetsInRange.count(pos) > 0;

        auto overTileSpr = [this, &tileSelected, &showPossibleTargets, &showTargetsInRange] {
            if (tileSelected) {
                return boost::optional<gf::Sprite&>{m_selectedTile};
            }

            if (showPossibleTargets) {
                return boost::optional<gf::Sprite&>{m_possibleTargetsTile};
            }

            if (showTargetsInRange) {
                return boost::optional<gf::Sprite&>{m_targetsInRangeTile};
            }

            return boost::optional<gf::Sprite&>{};
        }();

        if (overTileSpr) {
            overTileSpr->setPosition(gameToScreenPos(pos));
            batch.draw(*overTileSpr);
        }
    });
    batch.end();
}

void Game::switchTurn()
{
    if (m_board.getPlayingTeam() == m_humanPlayer.getTeam()) {
        m_selectedPos = boost::none;
        stateSelectionUpdate(PlayerTurnSelection::NoSelection);
        m_humanPlayer.setMoved(false);
    }

    m_board.switchTurn();
}
