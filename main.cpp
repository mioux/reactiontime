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
#include <assert.h>

#include "Console.h"
#include "main.h"
#include "displaystrings.h"

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
 * Implementation of sleep for DOS.
 */

int timeConsumingSleep(long long timeToWait)
{
    timeval *startTime = (timeval *)malloc(sizeof(struct timeval));
    timeval *diffTime = (timeval *)malloc(sizeof(struct timeval));

    gettimeofday(startTime, NULL);
    long long waitedTime = 0L;
    while (waitedTime < timeToWait)
    {
        gettimeofday(diffTime, NULL);
        waitedTime = totalMilliseconds(diffTime, startTime);
        cout << waitedTime << endl;
    }
    string test;
    cin >> test;
}

/*
 * Get a random time to wait before start.
 */

int getRandomTimeToWait()
{
    return (int) (101.0 * rand() / (RAND_MAX));
}

/*
 * Score between 0 and MAX_SCORE
 */

short score(long long realScore)
{
    return realScore < MAX_SCORE ? (short)realScore : MAX_SCORE;
}

/*
 * Difference in micromilliseconds of 2 timevals.
 * Remarks : In long, because total time should overflow.
 */
long long totalMilliseconds(const timeval * const start, const timeval * const end)
{
    long long totalStart = start->tv_usec + 1000000L * start->tv_sec;
    long long totalEnd = end->tv_usec + 1000000L * end->tv_sec;

    return totalStart - totalEnd;
}

/*
 * Play solo
 */

long long reactionTimeSolo()
{
    raw();

    // Structures for precise time.
    timeval *startTime = (timeval *)malloc(sizeof(struct timeval));
    timeval *endTime = (timeval *)malloc(sizeof(struct timeval));
    timeval *diffTime = (timeval *)malloc(sizeof(struct timeval));

    if (startTime == NULL || endTime == NULL || diffTime == NULL)
    {
        cerr << ERRER_WITH_TIME << endl;
        exit(EXIT_FAILURE);
    }

    con->setScore1(READY);
    con->win_refresh();

    halfdelay(getRandomTimeToWait());

    bool waitTimeExpired = false;
    while (false == waitTimeExpired)
    {
        int button = getch();
        if (ERR != button)
        {
            _firstPlayerFoul = true;
            halfdelay(getRandomTimeToWait());
            con->setScore1(FOUL);
            con->win_refresh();
        }
        else
        {
            waitTimeExpired = true;
        }
    }

    raw();
    con->setScore1(GO);
    con->win_refresh();
    gettimeofday(startTime, NULL);

    bool keyHit = false;
    while (false == keyHit)
    {
        int key = getch();
        if (p1Switch == key)
            keyHit = true;
    }
    gettimeofday(endTime, NULL);

    long long totalTime = totalMilliseconds(endTime, startTime) / 1000L;

    free(startTime);
    free(endTime);
    free(diffTime);

    startTime = NULL;
    endTime = NULL;
    diffTime = NULL;

    con->setScore1(score(totalTime));
    con->win_refresh();

    return totalTime;
}

/*
 * Two player run
 */

void reactionTimeDuo(long long &currentTimeP1, long long &currentTimeP2)
{
    currentTimeP1 = 0L;
    currentTimeP2 = 0L;

    timeval *startTime = (timeval *)malloc(sizeof(struct timeval));
    timeval *endTime = (timeval *)malloc(sizeof(struct timeval));
    timeval *diffTime = (timeval *)malloc(sizeof(struct timeval));

    if (startTime == NULL || endTime == NULL || diffTime == NULL)
    {
        cerr << "Error creating time structure" << endl;
        exit(EXIT_FAILURE);
    }

    con->setScore1(READY);
    con->setScore2(READY);
    con->win_refresh();
    
    halfdelay(getRandomTimeToWait());
    bool waitExpired = false;

    while (false == waitExpired)
    {
        int button = getch();
        if (p1Switch == button)
        {
            _firstPlayerFoul = true;
            con->setScore1(FOUL);
            con->win_refresh();
            halfdelay(getRandomTimeToWait());
        }
        else if (p2Switch == button)
        {
            _secondPlayerFoul = true;
            con->setScore2(FOUL);
            con->win_refresh();
            halfdelay(getRandomTimeToWait());
        }
        else if (ERR == button)
        {
            waitExpired = true;
        }
    }

    raw();
    con->setScore1(GO);
    con->setScore2(GO);
    con->win_refresh();

    gettimeofday(startTime, NULL);

    while (0L == currentTimeP1 || 0L == currentTimeP2)
    {
        int key = getch();
        if ((p1Switch == key) && 0L == currentTimeP1)
        {
            gettimeofday(endTime, NULL);
            currentTimeP1 = totalMilliseconds(endTime, startTime) / 1000L;
            con->setScore1(score(currentTimeP1));
            con->win_refresh();
        }
        else if ((p2Switch == key) && 0L == currentTimeP2)
        {
            gettimeofday(endTime, NULL);
            currentTimeP2 = totalMilliseconds(endTime, startTime) / 1000L;
            con->setScore2(score(currentTimeP2));
            con->win_refresh();
        }
    }

    free(startTime);
    free(endTime);
    free(diffTime);

    startTime = NULL;
    endTime = NULL;
    diffTime = NULL;
}

/*
 * One player game.
 */

void onePlayerGame()
{
    raw();
    con->setMarquee("1P game");
    con->win_refresh();

    reactionTimeSolo();

    timeConsumingSleep(2000);
}

/*
 * Two player game.
 */

void twoPlayerGame()
{
    raw();
    con->setMarquee("2P game");
    con->win_refresh();
    
    long long score1;
    long long score2;
    reactionTimeDuo(score1, score2);

    timeConsumingSleep(2000);
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
            con->setScore1(HIGH_SCORE);
            con->setScore2(highScore);
            break;
        case 2:
            // Average score
            con->setScore1(AVERAGE_SCORE);
            con->setScore2(averageScore);
            break;
        case 3:
            // Nb of played games
            con->setScore1(NUMBER_OF_PLAYED_GAMES);
            con->setScore2(nbGamesPlayed);
            break;
        case 4:
            // Nb of amaizing games
            con->setScore1(AMAIZING_GAMES);
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
    bool doStop = false;

    while (false == doStop)
    {
        con->setMarquee(INSERT_COIN);

        attractMode(cycle);

        con->win_refresh();

        halfdelay(50);
        int button = getch();

        if (creditKey == button)
        {
            con->setMarquee(WAITING_FOR_PLAYER_2);
            con->setScore1("");
            con->setScore2("");
            con->win_refresh();

            button = getch();

            if (creditKey == button)
                twoPlayerGame();
            else
                onePlayerGame();

            cycle = 4;
        }
        else if (quitKey == button)
        {
            doStop = true;
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
    bool doStop = false;

    while (false == doStop)
    {
        con->setMarquee(PRESS_START);

        attractMode(cycle);

        con->win_refresh();

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
        else if (quitKey == button)
        {
            doStop = true;
        }
            
        cycle = (cycle + 1) % 5;
    }
}

/**
 * Error when reading configuration.
 */

void errorReadingConfigFile()
{
    cerr << ERR_CONFIG_READ;
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
        cerr << ERR_CONFIG_WRITE;
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

    cout << "test wait 1" << endl;
    timeConsumingSleep(2000);
    cout << "test wait 2" << endl;
    timeConsumingSleep(2000);

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

    con->setMarquee(INSERT_COIN);
    con->setScore1(lastScoreP1);
    con->setScore2(lastScoreP2);
    
    if(false == freeGame)
        nonFreeGameLoop();
    else
        freeGameLoop();
    
    endwin();

    return EXIT_SUCCESS;
}
