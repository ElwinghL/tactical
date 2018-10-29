#include "game.h"

int main() {
    Game game {};

    while (game.isRunning()) {
        game.processEvents();
        game.update();
        game.render();
    }

    return 0;
}
