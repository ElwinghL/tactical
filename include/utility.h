/**
 * A file where utility functions and types are defined
 * \author Fabien Matusalem
 */
#ifndef UTILITY_H
#define UTILITY_H

#include <gf/Array2D.h>
#include <gf/Vector.h>
#include <gf/View.h>

#include <boost/optional.hpp>
#include <tuple>

/**
 * The different teams which fight in the game
 */
enum class PlayerTeam {
    Cthulhu, ///< The Cthulhu team
    Satan ///< The Satan team
};

class Character;
using Gameboard_t = gf::Array2D<boost::optional<Character>, int>;

/**
 * The type of the action done by a character
 *
 * \sa Action
 */
enum class ActionType {
    Attack, ///< The character attacks
    Capacity, ///< The character uses its capacity
    None ///< The character does nothing, i.e. it just moves
};

/**
 * The type of a character
 *
 * \sa Character
 */
enum class CharacterType {
    Tank, ///< The Tank, who is slow but grabs and blocks other characters
    Support, ///< The Support, who push its enemies and allies
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

/**
 * Position comparator
 *
 * Order positions by ascending x, then by ascending y
 */
struct PositionComp {
    /**
     * Compare the two positions
     *
     * \param lhs The first position
     * \param rhs The second position
     * \return True if the first position is "smaller" than the second one
     */
    bool operator()(const gf::Vector2i& lhs, const gf::Vector2i& rhs) const
    {
        return std::tie(lhs.x, lhs.y) < std::tie(rhs.x, rhs.y);
    }
};

void resizeView(gf::View& view, const gf::Vector2i& gameSize);
gf::Vector2f gameToScreenPos(const gf::Vector2i& gamePos);
gf::Vector2i screenToGamePos(const gf::Vector2f& screenPos);

#endif // UTILITY_H
