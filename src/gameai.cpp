#include "gameai.h"

void GameAI::simulateActions()
{
}


void GameAI::playTurn(gf::Array2D<Character*, int>* board)
{
    if (board == nullptr) {
        return;
    }
    std::vector<Action> allActions;
    for (auto it = m_characters.begin(); it != m_characters.end(); ++it) {
        std::vector<Action> thisCharacterActions = it->second.getPossibleActions(*board);
        for (auto it2 = thisCharacterActions.begin(); it2 != thisCharacterActions.end(); ++it2) {
            allActions.push_back(*it2);
        }
    }
    size_t randAction = rand() % allActions.size();
    allActions[randAction].execute(*board);
    std::cout << "Possible actions : " << allActions.size() << "\n";
}
