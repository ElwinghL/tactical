#include "gameai.h"


void GameAI::simulateActions()
{
    Gameboard board{};
    while (m_threadInput.pop(board)) {
        std::vector<Action> allActions;
        for (gf::Vector2i pos{0, 0}, size = board.getSize(); pos.x < size.width; ++pos.x) {
            for (pos.y = 0; pos.y < size.height; ++pos.y) {
                if (board.isOccupied(pos)) {
                    std::vector<Action> thisCharacterActions = board.getPossibleActions(pos);
                    for (auto& thisCharacterAction : thisCharacterActions) {
                        allActions.push_back(thisCharacterAction);
                    }
                }
            }
        }

        /*size_t randAction = rand() % allActions.size();

        using namespace std::literals;
        std::this_thread::sleep_for(5s); // TODO Replace this simulation

        m_threadOutput.push(allActions[randAction]);
        std::cout << "Possible actions : " << allActions.size() << "\n";*/

        /* TODO chnage the int value to get a more powerfull AI
         * 0 means best immediate action
         */
        GameAI::depthActionsExploration actionToDo = bestActionInFuture(board, 0);

//        if (actionToDo.first.getType() == ActionType::Attack) {
//            std::cout << "Attaque\n";
//        }
//        if (actionToDo.first.getType() == ActionType::Capacity) {
//            std::cout << "Capacité\n";
//        }
//        if (actionToDo.first.getType() == ActionType::None) {
//            std::cout << "None\n";
//        }


        board.display();
        actionToDo.first.display();
        m_threadOutput.push(actionToDo.first);
        std::cout << "Score = " << actionToDo.second.first << " Best score reached = " << actionToDo.second.second << "\n";
        std::cout << "Possible actions : " << allActions.size() << "\n";
    }
}

void GameAI::setInitialGameboard(const Gameboard& board)
{
    if (!m_initialBoardSet) {
        //m_threadInput.push(board);
    }
}

bool GameAI::playTurn(Gameboard& board)
{
    if (!m_waitingForThread) {
        m_threadInput.push(board);
        m_waitingForThread = true;
    }

    auto result = m_threadOutput.pop();
    if (result) {
        m_waitingForThread = false;
        result->execute(board);
    }

    return static_cast<bool>(result);
}

long GameAI::functionEval(const Gameboard& board)
{
    long score = 0;
    int enemyDamage = 0;

    std::vector<gf::Vector2i> myCharacterPositions, otherCharacterPositions;
    //find the differents characters positions
    for (int i = 0; i < board.getSize().x; ++i) {
        for (int j = 0; j < board.getSize().y; ++j) {
            gf::Vector2i pos = {i, j};
            if (board.isOccupied(pos)) {
                Character character = board.getCharacter(pos);
                if (character.getTeam() == getEnemyTeam(m_team)) {
                    enemyDamage += character.getHPMax() - character.getHP();
                }
                myCharacterPositions.push_back(pos);
            }
        }
    }

    score += enemyDamage;

    //Check if you can attack
    for (auto myPos : myCharacterPositions) {
        for (auto otherPos : otherCharacterPositions) {
            if (board.canAttack(myPos, otherPos)) {
                score += 10;
            }

            if (board.canAttack(myPos, otherPos)) {
                score -= 10;
            }
        }
    }

    int nbOfCharactersOnGoals = 0;

    //check if one player is on goal
    for (auto myGoal : m_goals) {
        if (myGoal.getTeam() == m_team) {
            if (myGoal.isActivated()) {
                score += 50;
                ++nbOfCharactersOnGoals;
            }
        }
    }

    int nbOfEnemyCharactersOnGoals = 0;

    for (auto enemyGoal : m_goals) {
        if (enemyGoal.getTeam() == getEnemyTeam(m_team)) {
            if (enemyGoal.isActivated()) {
                score -= 50;
                ++nbOfEnemyCharactersOnGoals;
            }
        }
    }

    if (nbOfCharactersOnGoals == nbOfGoalsPerPlayer) {
        return 9999;
    }

    if (nbOfEnemyCharactersOnGoals == nbOfGoalsPerPlayer) {
        return -9999;
    }

    //check if one player has lost some characters
    int nbOfDeadCharacters = 6 - static_cast<int>(myCharacterPositions.size());
    int nbOfEnemyDeadCharacters = 6 - static_cast<int>(otherCharacterPositions.size());

    if (nbOfDeadCharacters == 6 - 1) {
        return -9999;
    }

    if (nbOfEnemyDeadCharacters == 6 - 1) {
        return 9999;
    }

    if (nbOfDeadCharacters > 0) {
        score -= nbOfDeadCharacters * 10;
    }

    if (nbOfEnemyDeadCharacters > 0) {
        score += nbOfEnemyDeadCharacters * 10;
    }
    //Check if you can attack someone

    return score;
}

GameAI::depthActionsExploration GameAI::bestActionInFuture(Gameboard& board, unsigned int depth)
{
    std::vector<Action> allActions;
    for (gf::Vector2i pos{0, 0}, size = board.getSize(); pos.x < size.width; ++pos.x) {
        for (pos.y = 0; pos.y < size.height; ++pos.y) {
            if (board.isOccupied(pos) && board.getTeamFor(pos) == m_team) {
                std::vector<Action> thisCharacterActions = board.getPossibleActions(pos);
                for (auto& thisCharacterAction : thisCharacterActions) {
                    assert(thisCharacterAction.isValid(board));
                    allActions.push_back(thisCharacterAction);
                }
            }
        }
    }
    std::vector<Gameboard> boardsToAnalyse;

    for (auto actionAvailable : allActions) {
        assert(actionAvailable.isValid(board));
        Gameboard anOtherBoard{board};
        actionAvailable.execute(anOtherBoard);
        boardsToAnalyse.push_back(anOtherBoard);
    }

    long bestScore = -10000;
    Action bestAction = allActions.front();
    if (depth == 0) {
        long score = 0;
        for (auto actionAvailable : allActions) {
            if (actionAvailable.getType() != ActionType::None && actionAvailable.isValid(board)) {
                assert(actionAvailable.isValid(board));

                Gameboard anOtherBoard{board};
                actionAvailable.execute(anOtherBoard);
                score = functionEval(anOtherBoard);

                if (score > bestScore) {
                    bestAction = actionAvailable;
                    bestScore = score;
                }
            }
        }
        GameAI::depthActionsExploration actionToDo = std::make_pair(bestAction, std::make_pair(bestScore, bestScore));
//        if (actionToDo.first.getType() == ActionType::Attack) {
//            std::cout << "Attaque\n";
//        }
//        if (actionToDo.first.getType() == ActionType::Capacity) {
//            std::cout << "Capacité\n";
//        }
//        if (actionToDo.first.getType() == ActionType::None) {
//            std::cout << "None\n";
//        }
        //std::cout << "Bottom reached. " << bestScore << "\n";

        assert(actionToDo.first.isValid(board));
        return actionToDo;
    } else {
        std::vector<GameAI::depthActionsExploration> allPossibilities;
        long score = 0;
        for (auto actionAvailable : allActions) {
            Gameboard anOtherBoard{board};
            actionAvailable.execute(anOtherBoard);
            score = functionEval(anOtherBoard);
            boardsToAnalyse.push_back(anOtherBoard);
            if (score > bestScore) {
                bestAction = actionAvailable;
                bestScore = score;
            }
        }

        if (bestScore == 9999) {
            assert(bestAction.isValid(board));
            return std::make_pair(bestAction, std::make_pair(bestScore, bestScore));
        }

        for (auto currentBoard : boardsToAnalyse) {
            allPossibilities.push_back(bestActionInFuture(currentBoard, depth - 1));
        }
        long bestScoreRow = -10000; // So if the "best action" is to loose with a -9999 score it will be possible
        for (auto tab : allPossibilities) {
            if (tab.second.second > bestScoreRow && tab.first.isValid(board)) {
                bestScoreRow = tab.second.second;
                bestScore = tab.second.first;
                bestAction = tab.first;
            }
        }
        GameAI::depthActionsExploration actionToDo = std::make_pair(bestAction, std::make_pair(bestScore, bestScoreRow));
        std::cout << "Best score  = " << bestScore << " Best Score reached = " << bestScoreRow << "\n";
//            if (actionToDo.first.getType() == ActionType::Attack) {
//                std::cout << "Attaque\n";
//            }
//            if (actionToDo.first.getType() == ActionType::Capacity) {
//                std::cout << "Capacité\n";
//            }
//            if (actionToDo.first.getType() == ActionType::None) {
//                std::cout << "None\n";
//            }


        assert(actionToDo.first.isValid(board));
        //board.display();
        return actionToDo;
    }
}
