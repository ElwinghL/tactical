/**
 * This file provides the Action class
 * \author Fabien Matusalem
 */
#ifndef ACTION_H
#define ACTION_H

#include "utility.h"
#include "character.h"

#include <gf/Vector.h>
#include <gf/Array2D.h>

class Character;

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
     * \param character The character who is doing this action
     * \param type The type of this action
     * \param move A vector which is the difference between the start and the end of the movement
     * \param target The position which is the attack/capacity destination
     */
    Action(Character& character, ActionType type, const gf::Vector2i& move, const gf::Vector2i& target) :
        m_character{character},
        m_type{type},
        m_move{move},
        m_target{target}
    {
        // Nothing
    }

    /**
     * Constructor
     *
     * Make an action with only a movement and without attack/capacity
     *
     * \param character The character who is doing this action
     * \param move A vector which is the difference between the start and the end of the movement
     */
    Action(Character& character, const gf::Vector2i& move) :
        Action{character, ActionType::None, move, gf::Vector2i{0, 0}}
    {
        // Nothing
    }

    /**
     * Constructor
     *
     * Make an action without movement
     *
     * \param character The character who is doing this action
     * \param type The type of this action
     * \param target The position which is the attack/capacity destination
     */
    Action(Character& character, ActionType type, const gf::Vector2i& target) :
        Action{character, type, gf::Vector2i{0, 0}, target}
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
    bool isValid(gf::Array2D<Character*, int> board) const;

    /*
     * Execute this action. The character moves then attacks or uses a capacity
     */
    void execute(gf::Array2D<Character*, int> *board);
    
    gf::Vector2i getMove() const
    {
        return m_move;
    }
    
    gf::Vector2i getTarget() const
    {
        return m_target;
    }
    
    ActionType getType() const
    {
        return m_type;
    }

private:
    Character& m_character; ///< The character who is doing this action
    ActionType m_type; ///< The type of this action
    gf::Vector2i m_move; ///< The movement vector
    gf::Vector2i m_target; ///< The attack/capacity targeted position
};

#endif // ACTION_H
