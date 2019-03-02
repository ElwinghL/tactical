#include "gameai.h"

void GameAI::simulateActions()
{
    Gameboard board{};
    while (m_threadInput.pop(board)) {
        std::vector<Action> allActions;
        board.forEach([&board, &allActions](auto pos) {
            if (board.isOccupied(pos) && board.getTeamFor(pos) == board.getPlayingTeam()) {
                std::vector<Action> thisCharacterActions = board.getPossibleActions(pos);
                for (auto& thisCharacterAction : thisCharacterActions) {
                    allActions.push_back(thisCharacterAction);
                }
            }
        });

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

    std::vector<gf::Vector2i> myCharacterPositions{board.getTeamPositions(getTeam())};
    std::vector<gf::Vector2i> otherCharacterPositions{board.getTeamPositions(getEnemyTeam(getTeam()))};

    for (auto pos : otherCharacterPositions) {
        auto character = board.getCharacter(pos);
        enemyDamage = character.getHPMax() - character.getHP();
    }

    score += enemyDamage * 15;

    //Check if you can attack
    for (auto myPos : myCharacterPositions) {
        for (auto otherPos : otherCharacterPositions) {
            if (board.canAttack(myPos, otherPos)) {
                score += 5;
            }

            if (board.canAttack(otherPos, myPos)) {
                score -= 5;
            }

            if (board.capacityWillHurt(myPos, otherPos)) {
                score += 8;
            }

            if (board.capacityWillHurt(otherPos, myPos)) {
                score -= 8;
            }
        }
    }

    //check if one player is on goal
    score += 1000 * board.getNbOfActivatedGoals(getTeam());
    score -= 1000 * board.getNbOfActivatedGoals(getEnemyTeam(getTeam()));

    //check if one player has lost some characters
    int nbOfDeadCharacters = Gameboard::charactersPerTeam - static_cast<int>(myCharacterPositions.size());
    int nbOfEnemyDeadCharacters = Gameboard::charactersPerTeam - static_cast<int>(otherCharacterPositions.size());

    if (nbOfDeadCharacters == Gameboard::charactersPerTeam - 1) {
        return -9999;
    }

    if (nbOfEnemyDeadCharacters == Gameboard::charactersPerTeam - 1) {
        return 9999;
    }

    if (nbOfDeadCharacters > 0) {
        score -= nbOfDeadCharacters * 130;
    }

    if (nbOfEnemyDeadCharacters > 0) {
        score += nbOfEnemyDeadCharacters * 130;
    }
    std::array<int, 2> max = {0, 0};
    size_t i;
    //Get points if you're near a goal
    for (auto myPos : myCharacterPositions) {
        i = 0;
        for (auto dist : board.getGoalsDistance(myPos)) {
            max[i] = std::max(max[i], std::abs((125 - dist)));
            ++i;
        }
    }
    score += max[0];
    score += max[1];

    return score;
}

GameAI::depthActionsExploration GameAI::bestActionInFuture(Gameboard& board, unsigned int depth)
{
    std::vector<Action> allActions;
    board.forEach([this, &board, &allActions](auto pos) {
        if (board.isOccupied(pos) && board.getTeamFor(pos) == this->getTeam()) {
            std::vector<Action> thisCharacterActions = board.getPossibleActions(pos);
            for (auto& thisCharacterAction : thisCharacterActions) {
                assert(thisCharacterAction.isValid(board));
                allActions.push_back(thisCharacterAction);
            }
        }
    });
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
        long bestScoreRow = -10000; // So if the "best action" is to lose with a -9999 score it will be possible
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
