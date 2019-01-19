#include "gameai.h"

#include "action.h"

#include <chrono>

void GameAI::simulateActions()
{
    Gameboard_t currentBoard{};

    while (m_threadInput.pop(currentBoard)) {
        std::vector<Action> allActions;
        for (auto& square : currentBoard) {
            if (square && square->getTeam() == m_team) {
                Character& thisCharacter = *square;
                std::vector<Action> thisCharacterActions = thisCharacter.getPossibleActions(currentBoard);
                for (auto& action : thisCharacterActions) {
                    allActions.push_back(action);
                }
            }
        }
        size_t randAction = rand() % allActions.size();

        using namespace std::literals;
        std::this_thread::sleep_for(5s); // TODO Replace this simulation

        m_threadOutput.push(allActions[randAction]);
        std::cout << "Possible actions : " << allActions.size() << "\n";
    }
}

bool GameAI::playTurn(Gameboard_t& board)
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
