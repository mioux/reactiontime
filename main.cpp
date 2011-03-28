/* 
 * File:   main.cpp
 * Author: srumeu
 *
 * Created on 2 mars 2011, 12:49
 */

#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>
#include <string.h>
#include <sstream>
#include <fstream>
#include <curses.h>

#include "Console.h"
#include "main.h"

#define READY "Ready"
#define GO "GO !!!"
#define FOUL "FOUL !"
#define NOSCORE "XXXXXX"
#define FREE_GAME_SWITCH "--free-game"
#define MAX_SCORE 9999
#define CONFIG_FILE "rtime.cfg"

using namespace std;
/*
 * Has first player be too quick ?
 */
bool _firstPlayerFoul = false;

/*
 * Has second player be too quick ?
 */
bool _secondPlayerFoul = false;

/*
 * Score between 0 and MAX_SCORE
 */

char p1Start = '&';
char p2Start = 233;

char creditKey = '(';

char p1Switch = 'w';
char p2Switch = 'x';

char quitKey = 'q';

int nbGamesPlayed = 0;
int highScore = MAX_SCORE;
int averageScore = MAX_SCORE;
int amaizingScore = 0;

short lastScoreP1 = -1;
short lastScoreP2 = -1;

Console *con;

/*
 * One player game.
 */

void onePlayerGame()
{
    raw();
    con->setMarquee("1P game");

    halfdelay(50);
    getch();
}

/*
 * Two player game.
 */

void twoPlayerGame()
{
    raw();
    con->setMarquee("2P game");

    halfdelay(50);
    getch();
}

void attractMode(short cycle)
{
    switch (cycle)
    {
        case 0:
            // Last scores
            if (true == _firstPlayerFoul)
                con->setScore1(NOSCORE);
            else if (lastScoreP1 == -1)
                con->setScore1("");
            else
                con->setScore1(lastScoreP1);

            if (true == _secondPlayerFoul)
                con->setScore2(NOSCORE);
            else if (lastScoreP2 == -1)
                con->setScore2("");
            else
                con->setScore2(lastScoreP2);

            break;
        case 1:
            // Hish score
            con->setScore1("HISCOR");
            con->setScore2(highScore);
            break;
        case 2:
            // Average score
            con->setScore1("AVGSCR");
            con->setScore2(averageScore);
            break;
        case 3:
            // Nb of played games
            con->setScore1("PLAYED");
            con->setScore2(nbGamesPlayed);
            break;
        case 4:
            // Nb of amaizing games
            con->setScore1("WOW!!!");
            con->setScore2(amaizingScore);
            break;
    }
}

/*
 * Paying game.
 */

void nonFreeGameLoop()
{
    short cycle = 0;

    while (true)
    {
        con->setMarquee("Insert coins.net");

        attractMode(cycle);

        halfdelay(50);
        int button = getch();

        if (creditKey == button)
        {
            con->setMarquee("Wait for Player2");
            con->setScore1("");
            con->setScore2("");

            button = getch();

            if (creditKey == button)
                twoPlayerGame();
            else
                onePlayerGame();

            cycle = 4;
        }

        cycle = (cycle + 1) % 5;
    }
}

/*
 * Free game.
 */

void freeGameLoop()
{
    short cycle = 0;

    while (true)
    {
        con->setMarquee("Press start 1or2");

        attractMode(cycle);

        halfdelay(50);
        int button = getch();

        if (p1Start == button)
        {
            onePlayerGame();
            cycle = 4;
        }
        else if (p2Start == button)
        {
            twoPlayerGame();
            cycle = 4;
        }
            
        cycle = (cycle + 1) % 5;
    }
}

/**
 * Error when reading configuration.
 */

void errorReadingConfigFile()
{
    cerr << "Error reading config file.";
    exit(EXIT_FAILURE);
}

/*
 * Get char from config file.
 */

char getConfigFileChar(ifstream &configFile)
{
    string data;

    if (configFile.good())
        getline(configFile, data);
    else
        errorReadingConfigFile();

    return data[0];
}

/*
 * get int from config file.
 */

int getConfigFileInt(ifstream &configFile)
{
    string data;

    if (configFile.good())
        getline(configFile, data);
    else
        errorReadingConfigFile();

    return atoi(data.c_str());
}

/*
 * Writes default config file
 */

void writeConfigFile()
{
    ofstream configFile (CONFIG_FILE);
    if (configFile.is_open())
    {
        configFile << p1Start << endl;
        configFile << p2Start << endl;

        configFile << p1Switch << endl;
        configFile << p2Switch << endl;

        configFile << creditKey << endl;

        configFile << highScore << endl;
        configFile << nbGamesPlayed << endl;
        configFile << averageScore << endl;
        configFile << amaizingScore << endl;

        configFile.close();
    }
    else
    {
        cerr << "Error creating config file.";
        exit(EXIT_FAILURE);
    }
}

/*
 * Reads config file and set keys
 */

void readConfigFile()
{
    ifstream configFile (CONFIG_FILE);

    if (configFile.is_open())
    {
        p1Start = getConfigFileChar(configFile);
        p2Start = getConfigFileChar(configFile);

        p1Switch = getConfigFileChar(configFile);
        p2Switch = getConfigFileChar(configFile);
        
        creditKey = getConfigFileChar(configFile);

        highScore = getConfigFileInt(configFile);
        nbGamesPlayed = getConfigFileInt(configFile);
        averageScore = getConfigFileInt(configFile);
        amaizingScore = getConfigFileInt(configFile);

        configFile.close();
    }
    else
    {
        writeConfigFile();
    }
}

/*
 * Main function.
 */

int main(int argc, char *argv[])
{
    // init random for "real" random values
    srand(time(NULL));

    bool freeGame = false;

    readConfigFile();

    if (argc > 1)
    {
        string arg(argv[1]);
        string freeArg(FREE_GAME_SWITCH);
        if (arg.compare(freeArg) == 0)
            freeGame = true;
    }

    initscr();
    raw();
    noecho();
    curs_set(0);
    con = new Console();
    con->setMarquee("Insertcoins.net");
    con->setScore1(lastScoreP1);
    con->setScore2(lastScoreP2);
    
    if(false == freeGame)
        nonFreeGameLoop();
    else
        freeGameLoop();
    
    endwin();

    return EXIT_SUCCESS;
}
