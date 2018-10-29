#include "game.h"

#include <gf/ResourceManager.h>

int main() {
    gf::ResourceManager resMgr {};
    resMgr.addSearchDir("../assets/");

    Game game {&resMgr};

    while (game.isRunning()) {
        game.processEvents();
        game.update();
        game.render();
    }

    return 0;
}
