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
#include <conio.h>
#include <string.h>
#include <dos.h>
#include <sstream>
#include <fstream>

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

Console *con = new Console();;

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

short score(long long realScore)
{
    return realScore < MAX_SCORE ? (short)realScore : MAX_SCORE;
}

/*
 * My implementation of sleep.
 * This is time consuming as it does not turn the process in sleep mode
 * but is running for Xms
 */

bool timeConsumingSleep(int msTime, bool checkForFoul, bool checkForCredit)
{
    if (true == checkForFoul && true == checkForCredit)
    {
        cerr << "Error in parameters. Can't wait for foul & credits" << endl;
        exit(EXIT_FAILURE);
    }

    bool data = false;

    timeval *startTime = (timeval *)malloc(sizeof(struct timeval));
    timeval *curTime = (timeval *)malloc(sizeof (struct timeval));

    long long startTimeAsLong = 0L;
    long long curTimeAsLong = 0L;
    long long endTimeAsLong = 0L;

    if (startTime == NULL || curTime == NULL)
    {
        cerr << "Error creating time structure" << endl;
        exit(EXIT_FAILURE);
    }

    gettimeofday(startTime, NULL);
    startTimeAsLong = 1000L * startTime->tv_sec + startTime->tv_usec / 1000L;
    endTimeAsLong = startTimeAsLong + (long)msTime;
    curTimeAsLong = startTimeAsLong;

    while(curTimeAsLong < endTimeAsLong)
    {
        if (true == checkForFoul && kbhit() != 0)
        {
            int key = getch();
            if (p1Switch == key || p2Switch == key)
            {
                if (p1Switch == key)
                {
                    con->setScore1(FOUL);
                    _firstPlayerFoul = true;
                }
                else
                {
                    con->setScore2(FOUL);
                    _secondPlayerFoul = true;
                }
            }
        }
        else if (true == checkForCredit && kbhit() != 0)
        {
            int key = getch();
            if (creditKey == key)
            {
                data = true;
                break;
            }
        }

        gettimeofday(curTime, NULL);
        curTimeAsLong = 1000L * curTime->tv_sec + curTime->tv_usec / 1000L;
    }

    free(startTime);
    free(curTime);
    startTime = NULL;
    curTime = NULL;

    return data;
}

/*
 * timeConsumingSleep with checkForCredit set to false
 */

bool timeConsumingSleep(int msTime, bool checkForFoul)
{
    return timeConsumingSleep(msTime, checkForFoul, false);
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
    // "eating" typed chars
    while (kbhit() != 0)
    {
        getch();
    }

    // Structures for precise time.
    timeval *startTime = (timeval *)malloc(sizeof(struct timeval));
    timeval *endTime = (timeval *)malloc(sizeof(struct timeval));
    timeval *diffTime = (timeval *)malloc(sizeof(struct timeval));

    if (startTime == NULL || endTime == NULL || diffTime == NULL)
    {
        cerr << "Error creating time structure" << endl;
        exit(EXIT_FAILURE);
    }

    unsigned int timeToWait = (int) (10000.0 * rand() / (RAND_MAX + 1.0));

    timeConsumingSleep(timeToWait, true);

    con->setScore1(GO);
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

    return totalTime;
}

/*
 * Two player run
 */

void reactionTimeDuo(long long &currentTimeP1, long long &currentTimeP2)
{
    // "eating" typed chars
    while (kbhit() != 0)
    {
        getch();
    }
    
    timeval *startTime = (timeval *)malloc(sizeof(struct timeval));
    timeval *endTime = (timeval *)malloc(sizeof(struct timeval));
    timeval *diffTime = (timeval *)malloc(sizeof(struct timeval));

    if (startTime == NULL || endTime == NULL || diffTime == NULL)
    {
        cerr << "Error creating time structure" << endl;
        exit(EXIT_FAILURE);
    }

    unsigned int timeToWait = (int) (10000.0 * rand() / (RAND_MAX + 1.0));

    timeConsumingSleep(timeToWait, true);

    con->setScore1(GO);
    con->setScore2(GO);
    gettimeofday(startTime, NULL);

    while (0L == currentTimeP1 || 0L == currentTimeP2)
    {
        int key = getch();
        if ((p1Switch == key) && 0L == currentTimeP1)
        {
            gettimeofday(endTime, NULL);
            currentTimeP1 = totalMilliseconds(endTime, startTime) / 1000L;
            con->setScore1(score(currentTimeP1));
        }
        else if ((p2Switch == key) && 0L == currentTimeP2)
        {
            gettimeofday(endTime, NULL);
            currentTimeP2 = totalMilliseconds(endTime, startTime) / 1000L;
            con->setScore2(score(currentTimeP2));
        }
    }

    timeConsumingSleep(2000, false);

    free(startTime);
    free(endTime);
    free(diffTime);

    startTime = NULL;
    endTime = NULL;
    diffTime = NULL;
}

/*
 * Updates high score, number of played games, and average score.
 */
void updateScores(short score, bool hasFouled)
{
    averageScore = (averageScore * nbGamesPlayed + score) / (nbGamesPlayed + 1);
    ++nbGamesPlayed;
    if (false == hasFouled)
    {
        if (score < highScore)
            highScore = score;
        if (600 > score)
            ++amaizingScore;
    }
    writeConfigFile();
}

/*
 * Two player game
 */

void twoPlayerGame()
{
    _firstPlayerFoul = false;
    _secondPlayerFoul = false;

    lastScoreP1 = -1;
    lastScoreP2 = -1;

    long long totalTimeP1 = 0L;
    long long totalTimeP2 = 0L;

    con->setScore1("");
    con->setScore2("");

    for (int i = 0; i < 5; ++i)
    {
        long long currentTimeP1 = 0L;
        long long currentTimeP2 = 0L;
        
        ostringstream oss (ostringstream::out);
        if (0 == i)
            oss << "1st";
        else if (1 == i)
            oss << "2nd";
        else if (2 == i)
            oss << "3rd";
        else
            oss << i + 1 << "th";

        oss << " try";

        con->setMarquee(oss.str());

#ifndef DEBUG
        cout << '\a';
#endif
        con->setScore1(READY);
        con->setScore2(READY);
        reactionTimeDuo(currentTimeP1, currentTimeP2);

        totalTimeP1 += currentTimeP1;
        totalTimeP2 += currentTimeP2;

        if (i < 4)
            timeConsumingSleep(1000, false);
    }

    totalTimeP1 = score(totalTimeP1);
    totalTimeP2 = score(totalTimeP2);

    if (false == _firstPlayerFoul)
    {
        con->setScore1(totalTimeP1);
        lastScoreP1 = totalTimeP1;
    }
    else
    {
        con->setScore1(NOSCORE);
    }

    if (false == _secondPlayerFoul)
    {
        con->setScore2(totalTimeP2);
        lastScoreP2 = totalTimeP2;
    }
    else
    {
        con->setScore2(NOSCORE);
    }

    updateScores(totalTimeP1, _firstPlayerFoul);
    updateScores(totalTimeP2, _secondPlayerFoul);
}

/*
 * One player game
 */

void onePlayerGame()
{
    _firstPlayerFoul = false;
    _secondPlayerFoul = false;

    lastScoreP1 = -1;
    lastScoreP2 = -1;

    long long totalTime = 0L;

    con->setScore1("");
    con->setScore2("");

    for (int i = 0; i < 5; ++i)
    {
        ostringstream oss (ostringstream::out);
        if (0 == i)
            oss << "1st";
        else if (1 == i)
            oss << "2nd";
        else if (2 == i)
            oss << "3rd";
        else
            oss << i + 1 << "th";

        oss << " try";

        con->setMarquee(oss.str());

#ifndef DEBUG
        cout << '\a';
#endif
        
        con->setScore1(READY);
        long long curReationTime = reactionTimeSolo();
        con->setScore1(curReationTime);
        timeConsumingSleep(2000, false);
        totalTime += curReationTime;

        if (i < 4)
            timeConsumingSleep(1000, false);
    }

    totalTime = score(totalTime);

    if (false == _firstPlayerFoul)
    {
        con->setScore1(totalTime);
        lastScoreP1 = totalTime;
    }
    else
    {
        con->setScore1(NOSCORE);
    }

    updateScores(totalTime, _firstPlayerFoul);
}

/*
 * Free game main loop
 */

void freeGameLoop()
{
    while(true)
    {
        con->setMarquee("1->1P 2->2P");

        int startButton = getch();
        if (p1Start == startButton)
            onePlayerGame();
        else if (p2Start == startButton)
            twoPlayerGame();
        else if (quitKey == startButton)
            break;
    }
}

/*
 * Non free game main loop
 */

void nonFreeGameLoop()
{
    while (true)
    {
        short cycle = 0;

        while(1)
       {
            con->setMarquee("Insert coins.net");
            
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

            if (true == timeConsumingSleep(5000, false, true))
            {
                con->setMarquee("Wait for Player2");
                if (timeConsumingSleep(5000, false, true))
                {
                    twoPlayerGame();
                    cycle = 4;
                }
                else
                {
                    onePlayerGame();
                    cycle = 4;
                }
            }
            
            cycle = (cycle + 1) % 5;
        }
    }
}

/*
 * Error when reading config file. Exiting.
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

    if(false == freeGame)
        nonFreeGameLoop();
    else
        freeGameLoop();

    return EXIT_SUCCESS;
}
