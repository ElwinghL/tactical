#ifndef PLAYER_H
#define PLAYER_H

#include "character.h"
#include "goal.h"
#include "utility.h"

#include <array>
#include <functional>
#include <map>
#include <tuple>

class Player {
public:
    Player() = delete;

    Player(const Player&) = delete;
    Player(Player&&) = default;

    Player& operator=(const Player&) = delete;
    Player& operator=(Player&&) = delete;

    explicit Player(PlayerTeam team) :
        Player{team, true}
    {
        // Nothing
    }

    Player(PlayerTeam team, bool playFirst) :
        Player{team, playFirst, true}
    {
        // Nothing
    }

    virtual ~Player() noexcept = default;

    PlayerTeam getTeam() const
    {
        return m_team;
    }

    bool isTheirTurn() const
    {
        return m_theirTurn;
    }

    bool hasWon() const
    {
        return m_won;
    }

    void forEachCharacter(const std::function<void (Character*)>& doSomethingWith)
    {
        for (auto& characterEntry : m_characters) {
            doSomethingWith(&characterEntry.second);
        }
    }

protected:
    Player(PlayerTeam team, bool playFirst, bool closerCharacters);

private:
    struct PositionComp {
        bool operator()(const gf::Vector2i& lhs, const gf::Vector2i& rhs) const
        {
            return std::tie(lhs.x, lhs.y) < std::tie(rhs.x, rhs.y);
        }
    };

    const PlayerTeam m_team;

    bool m_theirTurn{true};
    bool m_won{false};

    std::map<gf::Vector2i, Character, PositionComp> m_characters;
    std::array<Goal, 2> m_goals;
};

#endif // PLAYER_H
