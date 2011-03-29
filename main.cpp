/* 
 * File:   main.cpp
 * Author: srumeu
 *
 * Created on 2 mars 2011, 12:49
 */

#include "Console.h"
#include "main.h"
#include "displaystrings.h"

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

unsigned char p1Start = '&';
unsigned char p2Start = 233;

unsigned char creditKey = '(';

unsigned char p1Switch = 'w';
unsigned char p2Switch = 'x';

unsigned char quitKey = 'q';

int nbGamesPlayed = 0;
int highScore = MAX_SCORE;
int averageScore = MAX_SCORE;
int amaizingScore = 0;
int nbGamesFouled = 0;

short lastScoreP1 = -1;
short lastScoreP2 = -1;

Console *con;

unsigned char redefineKey(string marquee)
{
    bool keyIsValid = false;
    int buffer = 0;

    raw();
    while (false == keyIsValid)
    {
        BEEP();
        con->setMarquee(marquee);
        con->win_refresh();
        buffer = getch();
        if (buffer < 256 && KEY_F(1) != buffer)
            keyIsValid = true;
    }

    return (unsigned char)buffer;
}

/*
 * Reconfigure keys.
 */

void keyConfig()
{
    unsigned char p1StartBuffer = 0;
    unsigned char p2StartBuffer = 0;

    unsigned char creditKeyBuffer = 0;

    unsigned char p1SwitchBuffer = 0;
    unsigned char p2SwitchBuffer = 0;

    unsigned char quitKeyBuffer = 0;

    raw();
    // Starts
    bool isValid = false;
    while (false == isValid)
    {
        p1StartBuffer = redefineKey(CONFIG_P1START);
        isValid = true;
    }
    isValid = false;
    while (false == isValid)
    {
        p2StartBuffer = redefineKey(CONFIG_P2START);
        isValid = p1StartBuffer != p2StartBuffer;
    }

    // Credit
    isValid = false;
    while (false == isValid)
    {
        creditKeyBuffer = redefineKey(CONFIG_CREDIT);
        isValid = p1StartBuffer   != creditKeyBuffer &&
                  p2StartBuffer   != creditKeyBuffer;
    }

    // Buttons
    isValid = false;
    while (false == isValid)
    {
        p1SwitchBuffer = redefineKey(CONFIG_P1BUTTON);
        isValid = p1StartBuffer   != p1SwitchBuffer &&
                  p2StartBuffer   != p1SwitchBuffer &&
                  creditKeyBuffer != p1SwitchBuffer;
    }
    isValid = false;
    while (false == isValid)
    {
        p2SwitchBuffer = redefineKey(CONFIG_P2BUTTON);
        isValid = p1StartBuffer   != p2SwitchBuffer &&
                  p2StartBuffer   != p2SwitchBuffer &&
                  creditKeyBuffer != p2SwitchBuffer &&
                  p1SwitchBuffer  != p2SwitchBuffer;
    }

    // Quit
    isValid = false;
    while (false == isValid)
    {
        quitKeyBuffer = redefineKey(CONFIG_QUIT);
        isValid = p1StartBuffer   != quitKeyBuffer &&
                  p2StartBuffer   != quitKeyBuffer &&
                  creditKeyBuffer != quitKeyBuffer &&
                  p1SwitchBuffer  != quitKeyBuffer &&
                  p2SwitchBuffer  != quitKeyBuffer;
    }

    p1Start = p1StartBuffer;
    p2Start = p2StartBuffer;

    creditKey = creditKeyBuffer;

    p1Switch = p1SwitchBuffer;
    p2Switch = p2SwitchBuffer;

    quitKey = quitKeyBuffer;

    writeConfigFile();
}

/*
 * Get current run number.
 */

string getRunNumber(int currentRunNumber, int totalRunNumber)
{
    ostringstream oss (ostringstream::out);
    oss << "Run " << currentRunNumber + 1 << "/" << totalRunNumber;
    return oss.str();
}

/*
 * Updates scores statistics.
 */

void updateScores(int score, bool foul)
{
    if (false == foul)
    {
        if (score < highScore)
            highScore = score;

        if (score < AMAIZING_SCORE)
            ++amaizingScore;

        averageScore = (averageScore * (nbGamesPlayed - nbGamesFouled) + score)
                                               /
                                (nbGamesPlayed - nbGamesFouled + 1);
    }

    ++nbGamesPlayed;
    if (true == foul)
        ++nbGamesFouled;

    writeConfigFile();
}

/*
 * Implementation of sleep for DOS.
 */

void timeConsumingSleep(long long timeToWait)
{
    timeval *startTime = (timeval *)malloc(sizeof(struct timeval));
    timeval *diffTime = (timeval *)malloc(sizeof(struct timeval));

    gettimeofday(startTime, NULL);
    long long waitedTime = 0L;
    while (waitedTime < timeToWait)
    {
        gettimeofday(diffTime, NULL);
        waitedTime = totalMilliseconds(diffTime, startTime);
    }
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
long long totalMilliseconds(const timeval * const end, const timeval * const start)
{
    long long totalEnd = end->tv_usec + 1000000L * end->tv_sec;
    long long totalStart = start->tv_usec + 1000000L * start->tv_sec;

    return (totalEnd - totalStart) / 1000L;
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

    if (startTime == NULL || endTime == NULL)
    {
        cerr << ERRER_WITH_TIME << endl;
        exit(EXIT_FAILURE);
    }

    BEEP();
    con->setScore1(READY);
    con->win_refresh();

    int timeToWait = getRandomTimeToWait();
    gettimeofday(startTime, NULL);
    halfdelay(timeToWait);

    bool waitTimeExpired = false;
    while (false == waitTimeExpired)
    {
        int button = getch();
        if (ERR != button)
        {
            _firstPlayerFoul = true;

            gettimeofday(endTime, NULL);
            timeToWait -= totalMilliseconds(endTime, startTime) / 100;
            timeToWait = timeToWait > 1 ? timeToWait : 1;
            con->win_refresh();
            halfdelay(timeToWait);

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

    long long totalTime = totalMilliseconds(endTime, startTime);

    free(startTime);
    free(endTime);

    startTime = NULL;
    endTime = NULL;

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

    if (startTime == NULL || endTime == NULL)
    {
        cerr << ERRER_WITH_TIME << endl;
        exit(EXIT_FAILURE);
    }

    BEEP();
    con->setScore1(READY);
    con->setScore2(READY);
    con->win_refresh();
    
    int timeToWait = getRandomTimeToWait();
    gettimeofday(startTime, NULL);
    halfdelay(timeToWait);
    bool waitExpired = false;

    while (false == waitExpired)
    {
        int button = getch();
        if (p1Switch == button)
        {
            _firstPlayerFoul = true;
            con->setScore1(FOUL);
            con->win_refresh();
        }
        else if (p2Switch == button)
        {
            _secondPlayerFoul = true;
            con->setScore2(FOUL);
            con->win_refresh();
        }
        else if (ERR == button)
        {
            waitExpired = true;
        }

        gettimeofday(endTime, NULL);
        timeToWait -= totalMilliseconds(endTime, startTime) / 100;
        timeToWait = timeToWait > 1 ? timeToWait : 1;
        halfdelay(timeToWait);
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
            currentTimeP1 = totalMilliseconds(endTime, startTime);
            con->setScore1(score(currentTimeP1));
            con->win_refresh();
        }
        else if ((p2Switch == key) && 0L == currentTimeP2)
        {
            gettimeofday(endTime, NULL);
            currentTimeP2 = totalMilliseconds(endTime, startTime);
            con->setScore2(score(currentTimeP2));
            con->win_refresh();
        }
    }

    free(startTime);
    free(endTime);

    startTime = NULL;
    endTime = NULL;
}

/*
 * One player game.
 */

void onePlayerGame()
{
    raw();
    con->setMarquee(ONE_PLAYER_GAME);
    con->win_refresh();

    long long totalTime = 0L;

    _firstPlayerFoul = false;

    for (int i = 0; i < RUN_NUMBER; ++i)
    {
        con->setMarquee(getRunNumber(i, RUN_NUMBER));
        con->win_refresh();
        
        long long currentTime = reactionTimeSolo();
        totalTime += currentTime;
        con->setScore1(score(currentTime));
        con->win_refresh();
        timeConsumingSleep(2000);
    }

    if (false == _firstPlayerFoul)
        lastScoreP1 = totalTime;
    else
        lastScoreP1 = -1;

    lastScoreP2 = -1;

    updateScores(totalTime, _firstPlayerFoul);
}

/*
 * Two player game.
 */

void twoPlayerGame()
{
    raw();
    con->setMarquee(TWO_PLAYER_GAME);
    con->win_refresh();

    long long totalScore1 = 0L;
    long long totalScore2 = 0L;

    for (int i = 0; i < RUN_NUMBER; ++i)
    {
        con->setMarquee(getRunNumber(i, RUN_NUMBER));
        con->win_refresh();

        long long currentScore1 = 0L;
        long long currentScore2 = 0L;
        reactionTimeDuo(currentScore1, currentScore2);

        con->setScore1(score(currentScore1));
        con->setScore2(score(currentScore2));
        con->win_refresh();

        totalScore1 += currentScore1;
        totalScore2 += currentScore2;
        timeConsumingSleep(2000);
    }

    if (false == _firstPlayerFoul)
        lastScoreP1 = totalScore1;
    else
        lastScoreP1 = -1;

    if (false == _secondPlayerFoul)
        lastScoreP2 = totalScore2;
    else
        lastScoreP2 = -1;

    updateScores(totalScore1, _firstPlayerFoul);
    updateScores(totalScore2, _secondPlayerFoul);
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
            // Nb of fouled games
            con->setScore1(FOULED);
            con->setScore2(nbGamesFouled);
            break;
        case 5:
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

            cycle = 5;
        }
        else if (KEY_F(1) == button)
        {
            keyConfig();
        }
        else if (quitKey == button)
        {
            doStop = true;
        }

        cycle = (cycle + 1) % 6;
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
            cycle = 5;
        }
        else if (p2Start == button)
        {
            twoPlayerGame();
            cycle = 5;
        }
        else if (quitKey == button)
        {
            doStop = true;
        }
        else if (KEY_F(1) == button)
        {
            keyConfig();
        }
            
        cycle = (cycle + 1) % 6;
    }
}

/**
 * Error when reading configuration.
 */

void errorReadingConfigFile()
{
    cerr << ERR_CONFIG_READ << endl;
    exit(EXIT_FAILURE);
}

/*
 * Get char from config file.
 */

unsigned char getConfigFileChar(ifstream &configFile)
{
    string data;

    if (configFile.good())
        getline(configFile, data);
    else
        errorReadingConfigFile();

    return (unsigned char)data[0];
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
        configFile << nbGamesFouled << endl;

        configFile.close();
    }
    else
    {
        cerr << ERR_CONFIG_WRITE << endl;
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
        nbGamesFouled = getConfigFileInt(configFile);

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
    keypad(stdscr, TRUE);
    con = new Console();
    
    if(false == freeGame)
        nonFreeGameLoop();
    else
        freeGameLoop();
    
    endwin();

    return EXIT_SUCCESS;
}
