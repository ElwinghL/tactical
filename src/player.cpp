#include "player.h"

Player::Player(PlayerTeam team, bool playFirst, bool closerCharacters) :
    m_team{team},
    m_theirTurn{playFirst},
    m_characters{},
    m_goals{[this, &closerCharacters] {
        int column{closerCharacters ? 1 : 10};

        return decltype(m_goals){Goal{m_team, {column, 1}},
                                 Goal{m_team, {column, 4}}};
    }()}
{
    gf::Vector2i pos{closerCharacters ? 2 : 9, 0};

    auto addCharacter{[this, &pos] (CharacterType type) {
        m_characters.emplace_hint(m_characters.cend(), pos, Character{m_team, type, pos});
        ++pos.y;
    }};

    addCharacter(CharacterType::Scout);
    addCharacter(CharacterType::Support);
    addCharacter(CharacterType::Tank);
    addCharacter(CharacterType::Tank);
    addCharacter(CharacterType::Support);
    addCharacter(CharacterType::Scout);
}
