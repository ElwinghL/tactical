#include "gameai.h"

#include <queue>

void GameAI::simulateActions()
{
    Gameboard currentBoard{};
    std::queue<Action> nextActions{};

    while (m_gameOpen) {
        // 1. Who is playing?
        if (currentBoard.getPlayingTeam() == getTeam()) {
            // 1.a. If an action is computed, send it
            if (!nextActions.empty()) {
                auto action = nextActions.front();

                assert(action.isValid(currentBoard));
                action.execute(currentBoard);
                currentBoard.switchTurn();

                action.display();
                m_threadOutput.push(std::move(action));
                nextActions.pop();
            }
        } else if (!m_threadInput.empty()) {
            // 1.b.1. Do the action
            auto inputBoard = m_threadInput.poll();

            // 1.b.2. Check the prediction if any
            if (!nextActions.empty()) {
                auto action = nextActions.front();

                action.execute(currentBoard);
                currentBoard.switchTurn();

                if (inputBoard == currentBoard) {
                    nextActions.pop();
                } else {
                    nextActions = std::queue<Action>{};
                    currentBoard = inputBoard;
                }
            }
        }

        // 2. Compute action
        // TODO change for saving actions according to a state
        if (nextActions.empty()) {
            depthActionsExploration actionToDo = bestActionInFuture(currentBoard, 0);
            nextActions.push(actionToDo.first);
//            nextActions.push(currentBoard.getPossibleActions()[0]);
            currentBoard.display();
//            std::cout << "Score = " << actionToDo.second.first << " Best score reached = " << actionToDo.second.second << "\n";
//            std::cout << "Possible actions : " << allActions.size() << "\n";
        }
    }
}

void GameAI::tryToPlay(Gameboard& board)
{
    if (!m_threadOutput.empty()) {
        Action action = m_threadOutput.poll();
        assert(action.isValid(board));
        action.execute(board);
        board.switchTurn();
    }
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
        auto goalDistances = board.getGoalsDistance(myPos);
        for (auto dist : goalDistances) {
            max[i] = std::max(max[i], std::abs((125 - dist)));
            ++i;
        }
    }
    score += max[0];
    score += max[1];

    return score;
}

GameAI::depthActionsExploration GameAI::bestActionInFuture(const Gameboard& board, unsigned int depth)
{
    std::vector<Action> allActions = board.getPossibleActions();
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
        depthActionsExploration actionToDo = std::make_pair(bestAction, std::make_pair(bestScore, bestScore));
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
        std::vector<depthActionsExploration> allPossibilities;
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

        for (const auto& currentBoard : boardsToAnalyse) {
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
        depthActionsExploration actionToDo = std::make_pair(bestAction, std::make_pair(bestScore, bestScoreRow));
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
