#ifndef PLAYER_H
#define PLAYER_H

#include "character.h"
#include "goal.h"
#include "utility.h"
#include <initializer_list>

#include <gf/Vector.h>

#include <array>
#include <functional>
#include <map>
#include <tuple>

constexpr std::size_t nbOfGoalsPerPlayer{2};

class Player {
public:
    Player() = delete;

    Player(const Player&) = delete;
    Player(Player&&) = default;

    Player& operator=(const Player&) = delete;
    Player& operator=(Player&&) = default;

    explicit Player(PlayerTeam team) :
        Player{team, true}
    {
        // Nothing
    }

    Player(PlayerTeam team, bool playFirst) :
        m_team{team},
        m_theirTurn{playFirst},
        m_characters{},
        m_goals{Goal{m_team, {0, 0}}, Goal{m_team, {0, 0}}}
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

    Character* addCharacter(Character&& character)
    {
        auto res{m_characters.emplace(character.getPosition(), character)};

        return res.second ? &res.first->second : nullptr;
    }

    void setGoalPositions(const std::array<gf::Vector2i, nbOfGoalsPerPlayer>& positions)
    {
        for (std::size_t i{0}; i < nbOfGoalsPerPlayer; ++i) {
            m_goals[i] = Goal{m_team, positions[i]};
        }
    }

    bool isOnAGoal(const Character& c) const;

private:
    struct PositionComp {
        bool operator()(const gf::Vector2i& lhs, const gf::Vector2i& rhs) const
        {
            return std::tie(lhs.x, lhs.y) < std::tie(rhs.x, rhs.y);
        }
    };

    PlayerTeam m_team;

    bool m_theirTurn{true};
    bool m_won{false};

    std::map<gf::Vector2i, Character, PositionComp> m_characters;
    std::array<Goal, nbOfGoalsPerPlayer> m_goals;
};

#endif // PLAYER_H
