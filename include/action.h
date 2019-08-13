/**
 * This file provides the Action class
 * \author Fabien Matusalem
 */
#ifndef ACTION_H
#define ACTION_H

#include "character.h"
#include "utility.h"

#include <gf/Array2D.h>
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
    Action(ActionType type, const gf::Vector2i& origin, const gf::Vector2i& dest, const gf::Vector2i& target) :
        m_type{type},
        m_origin{origin},
        m_dest{dest},
        m_target{target}
    {
        // Nothing
    }

    /**
     * Constructor
     *
     * Make an action with only a movement and without attack/capacity
     *
     * \param origin The position of the character executing the action
     * \param dest A vector which is the difference between the start and the end of the movement
     */
    Action(const gf::Vector2i& origin, const gf::Vector2i& dest) :
        Action{ActionType::None, origin, dest, origin}
    {
        // Nothing
    }

    /**
     * Constructor
     *
     * \param type The type of this action
     * \param origin The position of the character executing the action
     * \param target The position which is the attack/capacity destination
     */
    Action(ActionType type, const gf::Vector2i& origin, const gf::Vector2i& target) :
        Action{type, origin, origin, target}
    {
        // Nothing
    }

    /**
     * Tell if the action action is valid
     *
     * For example, an attack action is valid if the movement is not blocked, if it is
     * in the bounds and if there is an enemy character at the targeted position.
     * \param board The board of the game
     * \return True if this action is valid, false otherwise
     */
    bool isValid(const Gameboard& board) const;

    /**
     * Execute this action. The character moves then attacks or uses a capacity
     * \param board The board of the game
     */
    void execute(Gameboard& board) const;

    gf::Vector2i getDest() const
    {
        return m_dest;
    }

    gf::Vector2i getTarget() const
    {
        return m_target;
    }

    ActionType getType() const
    {
        return m_type;
    }

    gf::Vector2i getOrigin() const
    {
        return m_origin;
    }

    gf::Vector2i getMove() const
    {
        return m_dest - m_origin;
    }

    void display() const
    {
        if (m_origin != m_dest) {
            std::cout << "Move from (" << m_origin.x << ", " << m_origin.y << ") to (" << m_dest.x << ", " << m_dest.y
                      << ") then ";
        } else {
            std::cout << "From (" << m_origin.x << ", " << m_origin.y << "), ";
        }

        switch (m_type) {
        case ActionType::Attack:
            std::cout << "attack (" << m_target.x << ", " << m_target.y << ")" << std::endl;
            break;

        case ActionType::Capacity:
            std::cout << "use capacity on (" << m_target.x << ", " << m_target.y << ")" << std::endl;
            break;

        case ActionType::None:
            std::cout << "do nothing" << std::endl;
            break;
        }
    }

private:
    ActionType m_type; ///< The type of this action
    gf::Vector2i m_origin; ///< The position of the character who is doing this action
    gf::Vector2i m_dest; ///< The destination position of the character
    gf::Vector2i m_target; ///< The attack/capacity targeted position
};

#endif // ACTION_H
