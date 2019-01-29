/**
 * A file defining the game's AI
 * \author Fabien Matusalem
 */
#ifndef GAMEAI_H
#define GAMEAI_H

#include "action.h"
#include "gameboard.h"
#include "player.h"
#include "queues.h"
#include "utility.h"

#include <chrono>
#include <iostream>
#include <thread>
#include <utility.h>

/**
 * The artificial intelligence class
 *
 * This AI is based on the alpha-beta algorithm
 */
class GameAI : public Player {
public:
    using depthActionsExploration = std::pair<Action, std::pair<long, long>>; // FIXME Maybe tuple or struct?
    /**
     * Constructor
     * \param team The team the AI controls
     */
    explicit GameAI(PlayerTeam team) :
        Player{team, false}
    {
        // Nothing
    }

    virtual ~GameAI() noexcept
    {
        m_threadInput.fail();
        m_computingThread.join();
    }

    void setInitialGameboard(const Gameboard& board);

    bool playTurn(Gameboard& board);
    /**
     * \name: functionEval
     * \function: Give a score to a state (9999 means win and -9999 means defeat
     *
     * \param: Board game
     * \return : Score the actual configuration
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
    depthActionsExploration bestActionInFuture(Gameboard& board, unsigned int depth);

private:
    /**
     * Simulate the actions
     */
    void simulateActions();

    bool m_waitingForThread{false};
    bool m_initialBoardSet{false};


    std::thread m_computingThread{&GameAI::simulateActions, this};
    BlockingQueue<Gameboard> m_threadInput{};
    PollingQueue<Action> m_threadOutput{};
};

#endif // GAMEAI_H
