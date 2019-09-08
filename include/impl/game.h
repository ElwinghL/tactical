#ifndef IMPL_GAME_H
#define IMPL_GAME_H

[[nodiscard]] inline bool Game::isRunning()
{
    return m_window.isOpen();
}

[[nodiscard]] inline std::string Game::getName() const
{
    return "Cthulhu vs Satan";
}

[[nodiscard]] inline bool Game::isFromTeam(const gf::Vector2i& tile, PlayerTeam team) const
{
    return m_board.isOccupied(tile) && m_board.getCharacter(tile).getTeam() == team;
}

#endif //IMPL_GAME_H
