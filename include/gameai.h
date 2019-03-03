/**
 * A file defining the game's AI
 * \author Fabien Matusalem
 */
#ifndef GAMEAI_H
#define GAMEAI_H

#include "action.h"
#include "gameboard.h"
#include "player.h"
#include "pollingqueue.h"
#include "utility.h"

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

/**
 * The artificial intelligence class
 *
 * This AI is based on the alpha-beta algorithm
 */
class GameAI : public Player {
public:
    /**
     * Constructor
     * \param team The team the AI controls
     */
    explicit GameAI(PlayerTeam team) :
        Player{team}
    {
        // Nothing
    }

    virtual ~GameAI() noexcept
    {
        m_gameOpen = false;
        m_computingThread.join();
    }

    void askToPlay(const Gameboard& board)
    {
        m_threadInput.push(board);
    }

    void tryToPlay(Gameboard& board);

private:
    using depthActionsExploration = std::pair<Action, std::pair<long, long>>; // FIXME Maybe tuple or struct?

    /**
     * Give a score to a state (9999 means win and -9999 means defeat
     *
     * \param board Board game
     * \return Score of the actual configuration
     */
    long functionEval(const Gameboard& board);

    /**
     * The first int correspond to the depth of the configuration.
     * Pair is for the human player
     * Impair is for the AI player
     * @param board
     * @param depth
     * @return
     */
    depthActionsExploration bestActionInFuture(const Gameboard& board, unsigned int depth);

    /**
     * Simulate the actions
     */
    void simulateActions();

    std::atomic_bool m_gameOpen{true};

    std::thread m_computingThread{&GameAI::simulateActions, this};
    PollingQueue<Gameboard> m_threadInput{};
    PollingQueue<Action> m_threadOutput{};
};

#endif // GAMEAI_H
