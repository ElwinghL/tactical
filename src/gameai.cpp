#include "gameai.h"

#include "action.h"

#include <chrono>

void GameAI::simulateActions()
{
    Gameboard_t board{};
    while (m_threadInput.pop(board)) {
        std::vector<Action> allActions;
        for (auto& square : board) {
            if (square && square->getTeam() == m_team) {
                Character& thisCharacter = *square;
                std::vector<Action> thisCharacterActions = thisCharacter.getPossibleActions(board);
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

void GameAI::setInitialGameboard(const Gameboard_t& board)
{
    if (!m_initialBoardSet) {
        m_threadInput.push(board);
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
