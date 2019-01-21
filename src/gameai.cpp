#include <gameai.h>
#include <vector>
#include <utility.h>
#include <iostream>

#include "gameai.h"


void GameAI::simulateActions()
{
    Gameboard_t board{};
    while (m_threadInput.pop(board)) {
        std::vector<Action> allActions;
        for (boost::optional<Character>& m_character : board) {
            if(m_character && m_character->getTeam() == m_team) {
                std::vector<Action> thisCharacterActions = m_character->getPossibleActions(board);
                for (auto &thisCharacterAction : thisCharacterActions) {
                    allActions.push_back(thisCharacterAction);
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
        GameAI::depthActionsExploration actionToDo = bestActionInFuture(board,1);

        if (actionToDo.first.getType() == ActionType::Attack) {
            std::cout <<"Attaque\n";
        }
        if (actionToDo.first.getType() == ActionType::Capacity) {
            std::cout << "Capacité\n";
        }
        if (actionToDo.first.getType() == ActionType::None) {
            std::cout << "None\n";
        }

        m_threadOutput.push(actionToDo.first);
        std::cout << "Score = " << actionToDo.second.first << " Best score reached = " << actionToDo.second.second << "\n";
        std::cout << "Possible actions : " << allActions.size() << "\n";
    }
}

void GameAI::setInitialGameboard(const Gameboard_t &board)
{
    if (!m_initialBoardSet) {
        m_threadInput.push(board);
    }
}

bool GameAI::playTurn(Gameboard_t &board)
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

long GameAI::functionEval(const Gameboard_t &board)
{
    long score = 0;
    int nbOfCharactersOnGoals = 0;
    int nbOfEnemyCharactersOnGoals = 0;
    long nbOfDeadCharacters = 0;
    long nbOfEnemyDeadCharacters = 0;
    int maxHPSum = 0;

    std::vector < Character * > myCharacters, otherCharacters;
    //find the differents characters positions
    for (int i = 0; i < board.getSize().x; ++i) {
        for (int j = 0; j < board.getSize().y; ++j) {
            gf::Vector2i pos = {i, j};
            if (board.isValid(pos)) {
                if (board(pos)) {
                    if (board(pos)->getTeam() == m_team) {
                        Character character(m_team, board(pos)->getType(), pos);
                        myCharacters.push_back(&character);
                    } else {
                        Character character(getEnemyTeam(m_team), board(pos)->getType(), pos);
                        otherCharacters.push_back(&character);
                        maxHPSum += character.getHPMax();
                    }
                }
            }
        }
    }
    //Check if you can attack
    for (auto mCharacters : myCharacters) {
        for (auto oCharacters : otherCharacters) {
            if (mCharacters->canAttack(board, mCharacters->getPosition(),oCharacters->getPosition())) {
                score += 10;
            }
        }
    }
    for (auto oCharacters : otherCharacters) {
        for (auto mCharacters : myCharacters) {
            if (oCharacters->canAttack(board, oCharacters->getPosition(),mCharacters->getPosition())) {
                score -= 10;
            }
        }
    }
    //check if one player is on goal
    for (auto myGoal : m_goals) {
        if (myGoal.getTeam() == m_team) {
            if (myGoal.isActivated()) {
                score += 50;
                nbOfCharactersOnGoals++;
            }
        }
    }
    for (auto enemyGoal : m_goals) {
        if (enemyGoal.getTeam() == getEnemyTeam(m_team)) {
            if (enemyGoal.isActivated()) {
                score -= 50;
                nbOfEnemyCharactersOnGoals++;
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
    nbOfDeadCharacters = 6 - myCharacters.size();
    nbOfEnemyDeadCharacters = 6 - otherCharacters.size();
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

    int currHPSum = 0;
    for (auto oCharacters : otherCharacters) {
        currHPSum += oCharacters->getHP();
    }
    //Si l'ennemy est blessé.
    score += maxHPSum - currHPSum;

    return score;
}

GameAI::depthActionsExploration GameAI::bestActionInFuture(Gameboard_t &board, unsigned int Depth)
{
    std::vector<Action> allActions;
    for (boost::optional<Character>& m_character : board) {
        if(m_character && m_character->getTeam() == m_team) {
            std::vector<Action> thisCharacterActions = m_character->getPossibleActions(board);
            for (auto &thisCharacterAction : thisCharacterActions) {
                allActions.push_back(thisCharacterAction);
            }
        }
    }
    std::vector<Gameboard_t > boardsToAnalyse;

    for (auto actionAvailable : allActions) {
        Gameboard_t anOtherBoard{board};
        actionAvailable.execute(anOtherBoard);
        boardsToAnalyse.push_back(anOtherBoard);
    }

    long bestScore = -9998;
    Action bestAction = allActions.front();
    if (Depth == 0) {
        long score = 0;
        for (auto actionAvailable : allActions) {
            if (actionAvailable.getType() != ActionType::None) {
                Gameboard_t anOtherBoard{board};
                actionAvailable.execute(anOtherBoard);
                score = functionEval(anOtherBoard);

                if (score > bestScore) {
                    bestAction = actionAvailable;
                    bestScore = score;
                }
            }
        }
        GameAI::depthActionsExploration actionToDo = std::make_pair(bestAction,std::make_pair(bestScore,bestScore));
        if (actionToDo.first.getType() == ActionType::Attack) {
            std::cout <<"Attaque\n";
        }
        if (actionToDo.first.getType() == ActionType::Capacity) {
            std::cout << "Capacité\n";
        }
        if (actionToDo.first.getType() == ActionType::None) {
            std::cout << "None\n";
        }
        //std::cout << "Bottom reached. " << bestScore << "\n";

        return actionToDo;
    } else {

        std::vector<GameAI::depthActionsExploration> allPossibilities;
        long score = 0;
        for (auto actionAvailable : allActions) {
            Gameboard_t anOtherBoard{board};
            actionAvailable.execute(anOtherBoard);
            score = functionEval(anOtherBoard);
            boardsToAnalyse.push_back(anOtherBoard);
            if (score > bestScore) {
                bestAction = actionAvailable;
                bestScore = score;
            }
        }
        if (bestScore == 9999) {
            return std::make_pair(bestAction,std::make_pair(bestScore,bestScore));
        } else {
            for (auto currentBoard : boardsToAnalyse) {
                allPossibilities.push_back(bestActionInFuture(currentBoard,Depth-1));
            }
            long bestScoreRow = -10000; // So if the "best action" is to loose with a -9999 score it will be possible
            bestScore = -10000;
            for (auto tab : allPossibilities) {
                if (tab.second.second > bestScoreRow) {
                    bestScoreRow = tab.second.second;
                    bestScore = tab.second.first;
                    bestAction = tab.first;
                }
            }
            GameAI::depthActionsExploration actionToDo = std::make_pair(bestAction,std::make_pair(bestScore,bestScoreRow));
            std::cout << "Best score  =" << bestScore << " Best Score reached = " << bestScoreRow << "\n";
            if (actionToDo.first.getType() == ActionType::Attack) {
                std::cout <<"Attaque\n";
            }
            if (actionToDo.first.getType() == ActionType::Capacity) {
                std::cout << "Capacité\n";
            }
            if (actionToDo.first.getType() == ActionType::None) {
                std::cout << "None\n";
            }

            return actionToDo;
        }
    }
}
