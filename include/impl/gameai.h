#ifndef IMPL_GAMEAI_H
#define IMPL_GAMEAI_H

inline GameAI::GameAI(PlayerTeam team) :
    Player{team}
{
    // Nothing
}

inline GameAI::~GameAI() noexcept
{
    m_gameOpen = false;
    m_computingThread.join();
}

inline void GameAI::askToPlay(const Gameboard& board)
{
    m_threadInput.push(board);
}

#endif //IMPL_GAMEAI_H
