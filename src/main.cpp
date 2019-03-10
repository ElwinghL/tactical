#include "game.h"

#include <gf/ResourceManager.h>

int main()
{
    gf::ResourceManager resMgr{};
    resMgr.addSearchDir("../assets/");
    resMgr.addSearchDir("../assets/fonts");
    resMgr.addSearchDir("../assets/placeholders");
    resMgr.addSearchDir("../assets/characters");

    Game game{resMgr};

    while (game.isRunning()) {
        game.processEvents();
        game.update();
        game.render();
    }

    return 0;
}
