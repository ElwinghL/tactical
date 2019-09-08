/**
 * A file where utility functions and types are defined
 * \author Fabien Matusalem
 */
#ifndef UTILITY_H
#define UTILITY_H

#include <gf/Sprite.h>
#include <gf/Vector.h>
#include <gf/VectorOps.h>
#include <gf/View.h>

#include <optional>
#include <tuple>

/**
 * The different teams which fight in the game
 */
enum class PlayerTeam {
    Cthulhu, ///< The Cthulhu team
    Satan, ///< The Satan team
};

/**
 * The type of the action done by a character
 *
 * \sa Action
 */
enum class ActionType {
    Attack, ///< The character attacks
    Capacity, ///< The character uses its capacity
    None, ///< The character does nothing, i.e. it just moves
};

/**
 * The type of a character
 *
 * \sa Character
 */
enum class CharacterType {
    Tank, ///< The Tank, who is slow but grabs and blocks other characters
    Support, ///< The Support, who push its enemies and allies
    Scout, ///< The Scout, who is weak but fast
};

/**
 * Get the other team in the game
 *
 * \param team The "ally" team
 * \return Cthulhu if the team is Satan, else Satan
 *
 * \sa PlayerTeam
 */
constexpr PlayerTeam getEnemyTeam(PlayerTeam team);

constexpr bool isOrthogonal(const gf::Vector2i& v);
constexpr bool isOrthogonal(const gf::Vector2i& origin, const gf::Vector2i& dest);

constexpr bool isDiagonal(const gf::Vector2i& v);
constexpr bool isDiagonal(const gf::Vector2i& origin, const gf::Vector2i& dest);

void resizeView(gf::View& view, const gf::Vector2i& gameSize);
gf::Vector2f gameToScreenPos(const gf::Vector2i& gamePos);
gf::Vector2i screenToGamePos(const gf::Vector2f& screenPos);

constexpr gf::Vector2f gameToScreenPos(const gf::Vector2f& gamePos);

#ifdef SHOW_BOUNDING_BOXES
void showBoundingBox(const gf::Sprite& obj, gf::RenderTarget& target, const gf::RenderStates& states);
#endif // SHOW_BOUNDING_BOXES

constexpr gf::Vector2f halfTileSize{64.0f / 2.0f, 31.0f / 2.0f};

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
    constexpr bool operator()(const gf::Vector2i& lhs, const gf::Vector2i& rhs) const;
};

#include "impl/utility.h"

#endif // UTILITY_H
