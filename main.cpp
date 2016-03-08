/* 
 * File:   main.cpp
 * Author: srumeu
 *
 * Created on 2 mars 2011, 12:49
 */

#include "Console.h"
#include "main.h"
#include "displaystrings.h"
#include "Game.h"

int main(int argc, char *argv[]) {
    // init random for "real" random values
    srand(time(NULL));

    bool freeGame = false;

    if (argc > 1) {
        string arg(argv[1]);
        string freeArg(FREE_GAME_SWITCH);
        if (arg.compare(freeArg) == 0)
            freeGame = true;
    }

    Game *game = new Game();

    if (false == freeGame)
        game->NonFreeGameLoop();
    else
        game->FreeGameLoop();

    delete game;

    return EXIT_SUCCESS;
}
