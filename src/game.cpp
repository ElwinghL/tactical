#include "game.h"

#include "action.h"
#include "utility.h"

#include <gf/SpriteBatch.h>
#include <gf/VectorOps.h>

#include <algorithm>
#include <iostream>

#include <cstdlib>
#include <ctime>

Game::Game(gf::ResourceManager* resMgr) :
    m_resMgr{resMgr}
{
    srand(time(NULL)); // TODO Replace with gf::Random
    initWindow();
    initViews();
    initActions();
    initWidgets();
    initSprites();
    initEntities();
    initGoals();

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
        HumanPlayer* activePlayer = &m_humanPlayer;
        Player* enemyPlayer = &m_aiPlayer;
        if (m_leftClickAction.isActive()) {
            gf::Vector2i tile{screenToGamePos(m_mouseCoords)};
            //std::cout << "(" << tile.x << ", " << tile.y << ")" << std::endl;
            /*for(int i = 0; i < 12; ++i){
                for(int j = 0; j < 6; ++j){
                    if(m_board(gf::Vector2i(i,j))){
                        switch(m_board(gf::Vector2i(i,j))->getType()){
                            case CharacterType::Tank: {
                                std::cout << "T";
                                break;
                            }
                            case CharacterType::Support: {
                                std::cout << "s";
                                break;
                            }
                            case CharacterType::Scout: {
                                std::cout << "S";
                                break;
                            }
                        }
                    }
                }
                std::cout << "\n";
            }*/


            bool targetIsEmpty = false;
            if (m_selectedCharacter) {
                targetIsEmpty = (!getCharacter(tile));
            }

            if (m_selectedCharacter && m_buttonAttack.contains(m_mouseCoords)) {
                stateSelectionUpdate(PlayerTurnSelection::AttackSelection);
                break;
            }

            if (m_selectedCharacter && m_buttonCapacity.contains(m_mouseCoords)) {
                stateSelectionUpdate(PlayerTurnSelection::CapacitySelection);
                break;
            }

            if (m_selectedCharacter && m_buttonPass.contains(m_mouseCoords)) {
                stateSelectionUpdate(PlayerTurnSelection::NoSelection);
                m_selectedCharacter = nullptr;
                switchTurn();
                break;
            }

            switch (m_playerTurnSelection) {
            case PlayerTurnSelection::NoSelection: {
                if (!getCharacter(tile, activePlayer->getTeam())) {
                    m_selectedCharacter = nullptr;
                    stateSelectionUpdate(PlayerTurnSelection::NoSelection);
                } else {
                    m_selectedCharacter = getCharacter(tile, activePlayer->getTeam());
                    std::cout << tile.x << ":" << tile.y << "\n";
                    std::cout << m_selectedCharacter->getPosition().x << ":" << m_selectedCharacter->getPosition().y << "\n";
                    stateSelectionUpdate(PlayerTurnSelection::MoveSelection);
                }
            } break;

            case PlayerTurnSelection::MoveSelection: {
                if (getCharacter(tile, activePlayer->getTeam()) && getCharacter(tile, activePlayer->getTeam()) != m_selectedCharacter) {
                    m_selectedCharacter = getCharacter(tile, activePlayer->getTeam());
                    stateSelectionUpdate(PlayerTurnSelection::MoveSelection);
                } else if (m_selectedCharacter && targetIsEmpty && m_selectedCharacter->canMove(tile - m_selectedCharacter->getPosition(), m_board)) {
                    //std::cout << "(" << m_selectedCharacter->getPosition().x << ":" << m_selectedCharacter->getPosition().y << ")";
                    Action thisMove{*m_selectedCharacter, ActionType::None, tile - m_selectedCharacter->getPosition(), m_selectedCharacter->getPosition()};
                    thisMove.execute(m_board);
                    //std::cout << "(" << m_selectedCharacter->getPosition().x << ":" << m_selectedCharacter->getPosition().y << ")\n";
                    activePlayer->setMoved(true);
                    stateSelectionUpdate(PlayerTurnSelection::AttackSelection);
                } else {
                    m_selectedCharacter = nullptr;
                    stateSelectionUpdate(PlayerTurnSelection::NoSelection);
                }
            } break;

            case PlayerTurnSelection::AttackSelection: {
                if (!targetIsEmpty) {
                    Character* target = getCharacter(tile, enemyPlayer->getTeam());
                    if (target && m_selectedCharacter->attack(*target, m_board)) {
                        removeCharacterIfDead(tile);
                        m_selectedCharacter = nullptr;
                        stateSelectionUpdate(PlayerTurnSelection::NoSelection);
                        switchTurn();
                    } else if (!activePlayer->hasMoved()) {
                        m_selectedCharacter = nullptr;
                        stateSelectionUpdate(PlayerTurnSelection::NoSelection);
                    }
                } else if (!activePlayer->hasMoved()) {
                    m_selectedCharacter = nullptr;
                    stateSelectionUpdate(PlayerTurnSelection::NoSelection);
                }
            } break;

            case PlayerTurnSelection::CapacitySelection: {
                if (m_selectedCharacter) {
                    Action capacity{*m_selectedCharacter, ActionType::Capacity, tile};
                    if (capacity.isValid(m_board)) {
                        capacity.execute(m_board);
                        //Temporaire :
                        removeCharacterIfDead(tile);
                        removeCharacterIfDead(tile + gf::Vector2i(0, 1));
                        removeCharacterIfDead(tile + gf::Vector2i(0, -1));
                        removeCharacterIfDead(tile + gf::Vector2i(1, 0));
                        removeCharacterIfDead(tile + gf::Vector2i(-1, 0));

                        m_selectedCharacter = nullptr;
                        stateSelectionUpdate(PlayerTurnSelection::NoSelection);
                        switchTurn();
                    } else if (!activePlayer->hasMoved()) {
                        m_selectedCharacter = nullptr;
                        stateSelectionUpdate(PlayerTurnSelection::NoSelection);
                    }
                } else if (!activePlayer->hasMoved()) {
                    m_selectedCharacter = nullptr;
                    stateSelectionUpdate(PlayerTurnSelection::NoSelection);
                }
            } break;
            }
        }
    } break;

    case GameState::WaitingForAI: {
        if (m_aiPlayer.playTurn(m_board)) {
            //Temporaire :
            for (int i = 0; i < 6; ++i) {
                for (int j = 0; j < 12; ++j) {
                    gf::Vector2i thisPos(i, j);
                    removeCharacterIfDead(thisPos);
                }
            }

            switchTurn();
        }
    } break;

    case GameState::GameEnd: {
    } break;
    }

    m_actions.reset();
}

void Game::stateSelectionUpdate(PlayerTurnSelection nextState)
{
    //Mise à jour des cases à surligner visuellement :
    m_possibleTargets.clear();
    m_targetsInRange.clear();
    switch (nextState) {
    case PlayerTurnSelection::MoveSelection: {
        std::vector<Action> actions = m_selectedCharacter->getPossibleActions(m_board);
        for (size_t i = 0; i < actions.size(); ++i) {
            //std::cout << "Move : (" << m_selectedCharacter->getPosition().x+actions[i].getMove().x << ";" << m_selectedCharacter->getPosition().y+actions[i].getMove().y << ")\t";
            ActionType type = actions[i].getType();
            switch (type) {
            case ActionType::Attack: {
                //std::cout << "Attack : (" << actions[i].getTarget().x << ";" << actions[i].getTarget().y << ")";
            } break;

            case ActionType::Capacity: {
                //std::cout << "Capacity : (" << actions[i].getTarget().x << ";" << actions[i].getTarget().y << ")";
            } break;

            case ActionType::None:
                break;
            }
            //std::cout << "\n";
        }
        m_possibleTargets = m_selectedCharacter->getAllPossibleMoves(m_board);
        m_targetsInRange = m_selectedCharacter->getAllPossibleMoves(m_board, true);
    } break;
    case PlayerTurnSelection::AttackSelection: {
        m_possibleTargets = m_selectedCharacter->getAllPossibleAttacks(m_board);
        m_targetsInRange = m_selectedCharacter->getAllPossibleAttacks(m_board, true);
    } break;
    case PlayerTurnSelection::CapacitySelection: {
        m_possibleTargets = m_selectedCharacter->getAllPossibleCapacities(m_board);
        m_targetsInRange = m_selectedCharacter->getAllPossibleCapacities(m_board, true);
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

    case GameState::Pause: {
    } break;

    case GameState::GameStart: {
        /*for (int y = 0; y < m_board.getRows(); ++y) {
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
        }*/

        m_clearColor = gf::Color::Black;

        m_gameState = GameState::PlayerTurn;
    } break;

    case GameState::PlayerTurn:
    case GameState::WaitingForAI: {
        updateGoals();

        if (m_aiPlayer.hasWon() || m_humanPlayer.hasWon()) {
            m_gameState = GameState::GameEnd;
        }
    } break;

    case GameState::GameEnd: {
        if (m_humanPlayer.hasWon()) {
            std::cout << "Le joueur a gagné !" << std::endl;
        } else {
            std::cout << "L'IA a gagné !" << std::endl;
        }
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
    } break;

    case GameState::GameStart: {
    } break;

    case GameState::PlayerTurn:
    case GameState::WaitingForAI: {
        m_renderer.setView(m_mainView);
        drawBackground();
        drawCharacters();
        drawUI();
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

void Game::initGoals()
{
    std::array<Goal, 2>& cthulhuGoal = m_humanPlayer.getGoals();
    m_goals.push_back(&(cthulhuGoal[0]));
    m_goals.push_back(&(cthulhuGoal[1]));
    std::array<Goal, 2>& satanGoal = m_aiPlayer.getGoals();
    m_goals.push_back(&(satanGoal[0]));
    m_goals.push_back(&(satanGoal[1]));
}

void Game::initViews()
{
    resizeView(m_mainView, getBoardSize());

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
        m_gameState = GameState::GameStart;
    });

    buttonInit(m_quitButton, gf::Anchor::TopCenter, {0.0f, 20.0f});
    m_quitButton.setCallback([this] {
        m_window.close();
    });
}

void Game::initSprites()
{
    m_brightTile.setAnchor(gf::Anchor::TopLeft);
    m_darkTile.setAnchor(gf::Anchor::TopLeft);
    m_selectedTile.setAnchor(gf::Anchor::TopLeft);
    m_possibleTargetsTile.setAnchor(gf::Anchor::TopLeft);
    m_targetsInRangeTile.setAnchor(gf::Anchor::TopLeft);
    m_goalCthulhu.setAnchor(gf::Anchor::TopLeft);
    m_goalCthulhuActivated.setAnchor(gf::Anchor::TopLeft);
    m_goalSatan.setAnchor(gf::Anchor::TopLeft);
    m_goalSatanActivated.setAnchor(gf::Anchor::TopLeft);

    m_buttonAttack.setAnchor(gf::Anchor::TopLeft);
    m_buttonCapacity.setAnchor(gf::Anchor::TopLeft);
    m_buttonPass.setAnchor(gf::Anchor::TopLeft);
}

void Game::initEntities()
{
    m_aiPlayer.setGoalPositions({gf::Vector2i{1, 1}, gf::Vector2i{1, 4}});
    m_humanPlayer.setGoalPositions({gf::Vector2i{10, 1}, gf::Vector2i{10, 4}});

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

void Game::addCharacter(Player& player, Character&& character)
{
    if (!m_board(character.getPosition())) {
        Character* tile = player.addCharacter(std::move(character));
        m_board(character.getPosition()) = *tile;
    }
}

Character* Game::getCharacter(gf::Vector2i pos, PlayerTeam team)
{
    if (positionIsValid(pos) && m_board(pos) && m_board(pos)->getTeam() == team) {
        return &*m_board(pos);
    }
    return nullptr;
}

Character* Game::getCharacter(gf::Vector2i pos)
{
    if (positionIsValid(pos) && m_board(pos)) {
        return &*m_board(pos);
    }
    return nullptr;
}

void Game::drawUI()
{
    if (m_selectedCharacter) {
        //Dans l'idée, ça serait plus ergonomique d'afficher les boutons en bas à droite
        gf::Vector2i posButtonAttack{0, 80};
        m_buttonAttack.setPosition(posButtonAttack);
        gf::Vector2i posButtonCapacity{35, 80};
        m_buttonCapacity.setPosition(posButtonCapacity);
        gf::Vector2i posButtonPass{70, 80};
        m_buttonPass.setPosition(posButtonPass);
        m_uiWidgets.render(m_renderer);
    }
}

void Game::drawBackground()
{
    const gf::Vector2i size{getBoardSize()};

    gf::SpriteBatch batch{m_renderer};
    batch.begin();
    for (int x{size.width - 1}; x >= 0; --x) {
        for (int y{0}; y < size.height; ++y) {
            bool selectedTile = (m_selectedCharacter && m_selectedCharacter->getPosition().x == x && m_selectedCharacter->getPosition().y == y);
            bool showPossibleTargets = (m_selectedCharacter && m_possibleTargets.end() != m_possibleTargets.find(gf::Vector2i{x, y}));
            bool showTargetsInRange = (m_selectedCharacter && m_targetsInRange.end() != m_targetsInRange.find(gf::Vector2i{x, y}));
            auto tileSpr = [this, &x, &y]() -> gf::Sprite& {
                auto foundGoal = std::find_if(m_goals.cbegin(), m_goals.cend(), [&x, &y](const Goal* goal) { return goal->getPosition() == gf::Vector2i{x, y}; });
                if (foundGoal != m_goals.cend()) {
                    if ((*foundGoal)->getTeam() == PlayerTeam::Cthulhu) {
                        return m_goalSatan;
                    } else {
                        return m_goalCthulhu;
                    }
                } else if ((x + y) % 2 == 0) {
                    return m_darkTile;
                } else {
                    return m_brightTile;
                }
            }();
            tileSpr.setPosition(gameToScreenPos({x, y}));
            batch.draw(tileSpr);

            auto overTileSpr = [this, &selectedTile, &showPossibleTargets, &showTargetsInRange] {
                if (selectedTile) {
                    return boost::optional<gf::Sprite&>{m_selectedTile};
                } else if (showPossibleTargets) {
                    return boost::optional<gf::Sprite&>{m_possibleTargetsTile};
                } else if (showTargetsInRange) {
                    return boost::optional<gf::Sprite&>{m_targetsInRangeTile};
                }

                return boost::optional<gf::Sprite&>{};
            }();

            if (overTileSpr) {
                overTileSpr->setPosition(gameToScreenPos({x, y}));
                batch.draw(*overTileSpr);
            }
        }
    }
    batch.end();
}

void Game::switchTurn()
{
    m_aiPlayer.switchTurn();
    m_humanPlayer.switchTurn();
    if (m_gameState == GameState::PlayerTurn) {
        m_gameState = GameState::WaitingForAI;
    } else if (m_gameState == GameState::WaitingForAI) {
        m_gameState = GameState::PlayerTurn;
        m_humanPlayer.setMoved(false);
    }
}

void Game::removeCharacterIfDead(const gf::Vector2i& target)
{
    if (positionIsValid(target)) {
        if (m_board(target) && m_board(target)->getHP() <= 0) {
            Player* thisPlayer = nullptr;
            PlayerTeam thisTeam = m_board(target)->getTeam();
            if (thisTeam == PlayerTeam::Cthulhu) {
                thisPlayer = &m_humanPlayer;
            } else {
                thisPlayer = &m_aiPlayer;
            }
            thisPlayer->removeDeadCharacters();
            m_board(target) = boost::none;
        }
    }
}

void Game::updateGoals()
{
    m_humanPlayer.activateGoals();
    m_aiPlayer.activateGoals();
}

void Game::drawCharacters()
{
    for (int i = 11; i >= 0; --i) {
        for (int j = 0; j < 6; ++j) {
            gf::Vector2i thisPos(i, j);
            if (m_board(thisPos)) {
                const Character& thisChar = *m_board(thisPos);
                std::string spriteName = "placeholders/character.png";
                switch (thisChar.getType()) {
                case CharacterType::Scout: {
                    spriteName = "placeholders/scout.png";
                    break;
                }
                case CharacterType::Tank: {
                    spriteName = "placeholders/tank.png";
                    break;
                }
                case CharacterType::Support: {
                    spriteName = "placeholders/support.png";
                    break;
                }
                }
                gf::Sprite sprite = gf::Sprite{m_resMgr->getTexture(spriteName)};
                if (thisChar.getTeam() == PlayerTeam::Cthulhu) {
                    sprite.setScale({-1.0f, 1.0f}); // Mirrored
                    sprite.setAnchor(gf::Anchor::CenterRight);
                } else {
                    sprite.setColor(gf::Color::Red); // Red variant
                    sprite.setAnchor(gf::Anchor::CenterLeft);
                }
                sprite.setOrigin(sprite.getOrigin() + gf::Vector2f{0.0f, -4.0f});
                sprite.setPosition(gameToScreenPos(thisPos));
                m_renderer.draw(sprite, gf::RenderStates());
            }
        }
    }
}
