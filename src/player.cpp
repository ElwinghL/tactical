#include "player.h"

void Player::removeDeadCharacters()
{
    for (auto it = m_characters.begin(); it != m_characters.end(); ++it) {
        if (it->second.getHP() <= 0) {
            m_characters.erase(it);
            return;
        }
    }
}

void Player::activateGoals()
{
    for (Goal& goal : m_goals) {
        if (goal.isActivated()) {
            continue;
        }

        for (auto& c : m_characters) {
            if (goal.getPosition() == c.second.getPosition() &&
                goal.getTeam() == m_team) {
                goal.activate();
                break;
            }
        }
    }
}
