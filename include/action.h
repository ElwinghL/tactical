/**
 * This file provides the Action class
 * \author Fabien Matusalem
 */
#ifndef ACTION_H
#define ACTION_H

#include "character.h"
#include "utility.h"

#include <gf/Vector.h>

#include <optional>

class Gameboard;

/**
 * Represent an action done by a character
 *
 * An action is a move followed by an attack or a capacity use
 *
 * \sa Character
 */
class Action {
public:
    /**
     * Constructor
     *
     * \param type The type of this action
     * \param origin The position of the character executing the action
     * \param dest The position the character will move to
     * \param target The position which is the attack/capacity destination
     */
    constexpr Action(ActionType type, const gf::Vector2i& origin, const gf::Vector2i& dest, const gf::Vector2i& target);

    /**
     * Constructor
     *
     * Make an action with only a movement and without attack/capacity
     *
     * \param origin The position of the character executing the action
     * \param dest A vector which is the difference between the start and the end of the movement
     */
    constexpr Action(const gf::Vector2i& origin, const gf::Vector2i& dest);

    /**
     * Constructor
     *
     * \param type The type of this action
     * \param origin The position of the character executing the action
     * \param target The position which is the attack/capacity destination
     */
    constexpr Action(ActionType type, const gf::Vector2i& origin, const gf::Vector2i& target);

    /**
     * Tell if the action action is valid
     *
     * For example, an attack action is valid if the movement is not blocked, if it is
     * in the bounds and if there is an enemy character at the targeted position.
     * \param board The board of the game
     * \return True if this action is valid, false otherwise
     */
    [[nodiscard]] bool isValid(const Gameboard& board) const;

    /**
     * Execute this action. The character moves then attacks or uses a capacity
     * \param board The board of the game
     */
    void execute(Gameboard& board) const;

    [[nodiscard]] constexpr gf::Vector2i getDest() const;
    [[nodiscard]] constexpr gf::Vector2i getTarget() const;
    [[nodiscard]] constexpr ActionType getType() const;
    [[nodiscard]] constexpr gf::Vector2i getOrigin() const;
    [[nodiscard]] constexpr gf::Vector2i getMove() const;

    void display() const;

private:
    ActionType m_type; ///< The type of this action
    gf::Vector2i m_origin; ///< The position of the character who is doing this action
    gf::Vector2i m_dest; ///< The destination position of the character
    gf::Vector2i m_target; ///< The attack/capacity targeted position
};

#include "impl/action.h"

#endif // ACTION_H
