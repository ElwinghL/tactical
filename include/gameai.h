/**
 * A file defining the game's AI
 * \author Fabien Matusalem
 */
#ifndef GAMEAI_H
#define GAMEAI_H

#include "action.h"
#include "player.h"
#include "queues.h"
#include "utility.h"

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
        Player{team, false}
    {
        // Nothing
    }

    virtual ~GameAI() noexcept
    {
        std::cout << 1 << std::endl;
        m_threadInput.fail();
        std::cout << 2 << std::endl;
        m_computingThread.detach();
        std::cout << 3 << std::endl;
    }

    void setInitialGameboard(const Gameboard_t& board);
    bool playTurn(Gameboard_t& board);

private:
    /**
     * Simulate the actions
     */
    void simulateActions();

    bool m_waitingForThread{false};
    bool m_initialBoardSet{false};

    std::thread m_computingThread{&GameAI::simulateActions, this};
    BlockingQueue<Gameboard_t> m_threadInput{};
    PollingQueue<Action> m_threadOutput{};
};

#endif // GAMEAI_H
