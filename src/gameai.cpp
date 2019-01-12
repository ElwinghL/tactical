#include "gameai.h"

#include "action.h"

void GameAI::simulateActions()
{
}


void GameAI::playTurn(gf::Array2D<boost::optional<Character>, int>& board)
{
    std::vector<Action> allActions;
    for (auto it = board.begin(); it != board.end(); ++it) {
        if (*it && (*it)->getTeam() == m_team) {
            Character thisCharacter = *(*it);
            std::vector<Action> thisCharacterActions = thisCharacter.getPossibleActions(board);
            for (auto it2 = thisCharacterActions.begin(); it2 != thisCharacterActions.end(); ++it2) {
                allActions.push_back(*it2);
            }
        }
    }
    size_t randAction = rand() % allActions.size();
    allActions[randAction].execute(board);
    std::cout << "Possible actions : " << allActions.size() << "\n";
}
