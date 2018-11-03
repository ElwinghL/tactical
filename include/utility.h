/**
 * A file where utility functions and types are defined
 * \author Fabien Matusalem
 */
#ifndef UTILITY_H
#define UTILITY_H

#include <gf/Vector.h>
#include <gf/View.h>

/**
 * The different teams which fight in the game
 */
enum class PlayerTeam {
    Cthulhu, ///< The Cthulhu team
    Satan ///< The Satan team
};

/**
 * The type of the action done by a character
 *
 * \sa Action
 */
enum class ActionType {
    Attack, ///< The character attacks
    Capacity1, ///< The character uses its first capacity
    Capacity2, ///< The character uses its second capacity
    None ///< The character does nothing, i.e. it just moves
};

/**
 * The type of a character
 *
 * \sa Character
 */
enum class CharacterType {
    Tank, ///< The Tank, who is slow but grabs and blocks other characters
    Support, ///< The Support, who push its enemies
    Scout ///< The Scout, who is weak but fast
};

/**
 * Get the other team in the game
 *
 * \param team The "ally" team
 * \return Cthulhu if the team is Satan, else Satan
 *
 * \sa PlayerTeam
 */
constexpr PlayerTeam getEnemyTeam(PlayerTeam team)
{
    return (team == PlayerTeam::Cthulhu) ? PlayerTeam::Satan : PlayerTeam::Cthulhu;
}

gf::Vector2f positionToView(const gf::Vector2i& gamePos);
void resizeView(gf::View& view, const gf::Vector2i& gameSize);

#endif // UTILITY_H
