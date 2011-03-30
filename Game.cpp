#include "Game.h"
#include "displaystrings.h"

using namespace std;

Game::Game() {
    this->_firstPlayerFoul = false;
    this->_secondPlayerFoul = false;

    this->_p1Start = '&';
    this->_p2Start = 233;

    this->_creditKey = '(';

    this->_p1Switch = 'w';
    this->_p2Switch = 'x';

    this->_quitKey = 'q';

    this->_nbGamesPlayed = 0;
    this->_highScore = MAX_SCORE;
    this->_averageScore = MAX_SCORE;
    this->_amaizingScore = 0;
    this->_nbGamesFouled = 0;

    this->_lastScoreP1 = -1;
    this->_lastScoreP2 = -1;

    initscr();
    raw();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    this->_con = new Console();
    ReadConfigFile();
}

Game::~Game() {
    delete this->_con;
    endwin();
}

/*
 * Show keys configured.
 */
void Game::ShowKeys(string marquee, unsigned char button1, unsigned char button2) {
    this->_con->setMarquee(marquee);
    ostringstream btn1Text(ostringstream::out);
    btn1Text << button1;
    ostringstream btn2Text(ostringstream::out);
    btn2Text << button2;
    this->_con->setScore1(btn1Text.str());
    this->_con->setScore2(btn2Text.str());
    this->_con->win_refresh();
    getch();
}

/*
 * Show current configured keys.
 */

void Game::ShowConfig() {
    halfdelay(20);

    this->ShowKeys(CONFIG_KEY_SHOW_START, this->_p1Start, this->_p2Start);
    this->ShowKeys(CONFIG_KEY_SHOW_BUTTON, this->_p1Switch, this->_p2Switch);
    this->ShowKeys(CONFIG_KEY_SHOW_CREDIT_QUIT, this->_creditKey, this->_quitKey);
}

/*
 * Redefines the keys.
 */

unsigned char Game::RedefineKey(string marquee) {
    bool keyIsValid = false;
    int buffer = 0;

    raw();
    while (false == keyIsValid) {
        BEEP();
        this->_con->setMarquee(marquee);
        this->_con->setScore1("");
        this->_con->setScore2("");
        this->_con->win_refresh();
        buffer = getch();
        if (buffer < 256 && KEY_F(1) != buffer && KEY_F(2) != buffer)
            keyIsValid = true;
    }

    return (unsigned char) buffer;
}

/*
 * Reconfigure keys.
 */

void Game::KeyConfig() {
    unsigned char p1StartBuffer = 0;
    unsigned char p2StartBuffer = 0;

    unsigned char creditKeyBuffer = 0;

    unsigned char p1SwitchBuffer = 0;
    unsigned char p2SwitchBuffer = 0;

    unsigned char quitKeyBuffer = 0;

    raw();
    // Starts
    bool isValid = false;
    while (false == isValid) {
        p1StartBuffer = this->RedefineKey(CONFIG_P1START);
        isValid = true;
    }
    isValid = false;
    while (false == isValid) {
        p2StartBuffer = this->RedefineKey(CONFIG_P2START);
        isValid = p1StartBuffer != p2StartBuffer;
    }

    // Credit
    isValid = false;
    while (false == isValid) {
        creditKeyBuffer = this->RedefineKey(CONFIG_CREDIT);
        isValid = p1StartBuffer != creditKeyBuffer &&
                p2StartBuffer != creditKeyBuffer;
    }

    // Buttons
    isValid = false;
    while (false == isValid) {
        p1SwitchBuffer = this->RedefineKey(CONFIG_P1BUTTON);
        isValid = p1StartBuffer != p1SwitchBuffer &&
                p2StartBuffer != p1SwitchBuffer &&
                creditKeyBuffer != p1SwitchBuffer;
    }
    isValid = false;
    while (false == isValid) {
        p2SwitchBuffer = this->RedefineKey(CONFIG_P2BUTTON);
        isValid = p1StartBuffer != p2SwitchBuffer &&
                p2StartBuffer != p2SwitchBuffer &&
                creditKeyBuffer != p2SwitchBuffer &&
                p1SwitchBuffer != p2SwitchBuffer;
    }

    // Quit
    isValid = false;
    while (false == isValid) {
        quitKeyBuffer = this->RedefineKey(CONFIG_QUIT);
        isValid = p1StartBuffer != quitKeyBuffer &&
                p2StartBuffer != quitKeyBuffer &&
                creditKeyBuffer != quitKeyBuffer &&
                p1SwitchBuffer != quitKeyBuffer &&
                p2SwitchBuffer != quitKeyBuffer;
    }

    this->_p1Start = p1StartBuffer;
    this->_p2Start = p2StartBuffer;

    this->_creditKey = creditKeyBuffer;

    this->_p1Switch = p1SwitchBuffer;
    this->_p2Switch = p2SwitchBuffer;

    this->_quitKey = quitKeyBuffer;

    this->WriteConfigFile();
}

/*
 * Get current run number.
 */

string Game::GetRunNumber(int currentRunNumber, int totalRunNumber) {
    ostringstream oss(ostringstream::out);
    oss << "Run " << currentRunNumber + 1 << "/" << totalRunNumber;
    return oss.str();
}

/*
 * Updates scores statistics.
 */

void Game::UpdateScores(int score, bool foul) {
    if (false == foul) {
        if (score < this->_highScore)
            this->_highScore = score;

        if (score < AMAIZING_SCORE)
            ++this->_amaizingScore;

        this->_averageScore = (this->_averageScore * (this->_nbGamesPlayed - this->_nbGamesFouled) + score)
                /
                (this->_nbGamesPlayed - this->_nbGamesFouled + 1);
    }

    ++this->_nbGamesPlayed;
    if (true == foul)
        ++this->_nbGamesFouled;

    this->WriteConfigFile();
}

/*
 * Implementation of sleep for DOS.
 */

void Game::TimeConsumingSleep(long long timeToWait) {
    timeval *startTime = (timeval *) malloc(sizeof (struct timeval));
    timeval *diffTime = (timeval *) malloc(sizeof (struct timeval));

    gettimeofday(startTime, NULL);
    long long waitedTime = 0L;
    while (waitedTime < timeToWait) {
        gettimeofday(diffTime, NULL);
        waitedTime = this->TotalMilliseconds(diffTime, startTime);
    }
}

/*
 * Get a random time to wait before start.
 */

int Game::GetRandomTimeToWait() {
    return (int) (101.0 * rand() / (RAND_MAX));
}

/*
 * Score between 0 and MAX_SCORE
 */

short Game::Score(long long realScore) {
    return realScore < MAX_SCORE ? (short) realScore : MAX_SCORE;
}

/*
 * Difference in micromilliseconds of 2 timevals.
 * Remarks : In long, because total time should overflow.
 */
long long Game::TotalMilliseconds(const timeval * const end, const timeval * const start) {
    long long totalEnd = end->tv_usec + 1000000L * end->tv_sec;
    long long totalStart = start->tv_usec + 1000000L * start->tv_sec;

    return (totalEnd - totalStart) / 1000L;
}

/*
 * Play solo
 */

long long Game::ReactionTimeSolo() {
    raw();

    // Structures for precise time.
    timeval *startTime = (timeval *) malloc(sizeof (struct timeval));
    timeval *endTime = (timeval *) malloc(sizeof (struct timeval));

    if (startTime == NULL || endTime == NULL) {
        cerr << ERRER_WITH_TIME << endl;
        exit(EXIT_FAILURE);
    }

    BEEP();
    this->_con->setScore1(READY);
    this->_con->win_refresh();

    int timeToWait = this->GetRandomTimeToWait();
    gettimeofday(startTime, NULL);
    halfdelay(timeToWait);

    bool waitTimeExpired = false;
    while (false == waitTimeExpired) {
        int button = getch();
        if (ERR != button) {
            this->_firstPlayerFoul = true;

            gettimeofday(endTime, NULL);
            timeToWait -= TotalMilliseconds(endTime, startTime) / 100;
            timeToWait = timeToWait > 1 ? timeToWait : 1;
            this->_con->win_refresh();
            halfdelay(timeToWait);

            this->_con->setScore1(FOUL);
            this->_con->win_refresh();
        } else {
            waitTimeExpired = true;
        }
    }

    raw();
    this->_con->setScore1(GO);
    this->_con->win_refresh();
    gettimeofday(startTime, NULL);

    bool keyHit = false;
    while (false == keyHit) {
        int key = getch();
        if (_p1Switch == key)
            keyHit = true;
    }
    gettimeofday(endTime, NULL);

    long long totalTime = TotalMilliseconds(endTime, startTime);

    free(startTime);
    free(endTime);

    startTime = NULL;
    endTime = NULL;

    this->_con->setScore1(Score(totalTime));
    this->_con->win_refresh();

    return totalTime;
}

/*
 * Two player run
 */

void Game::ReactionTimeDuo(long long &currentTimeP1, long long &currentTimeP2) {
    currentTimeP1 = 0L;
    currentTimeP2 = 0L;

    timeval *startTime = (timeval *) malloc(sizeof (struct timeval));
    timeval *endTime = (timeval *) malloc(sizeof (struct timeval));

    if (startTime == NULL || endTime == NULL) {
        cerr << ERRER_WITH_TIME << endl;
        exit(EXIT_FAILURE);
    }

    BEEP();
    this->_con->setScore1(READY);
    this->_con->setScore2(READY);
    this->_con->win_refresh();

    int timeToWait = this->GetRandomTimeToWait();
    gettimeofday(startTime, NULL);
    halfdelay(timeToWait);
    bool waitExpired = false;

    while (false == waitExpired) {
        int button = getch();
        if (this->_p1Switch == button) {
            this->_firstPlayerFoul = true;
            this->_con->setScore1(FOUL);
            this->_con->win_refresh();
        } else if (this->_p2Switch == button) {
            this->_secondPlayerFoul = true;
            this->_con->setScore2(FOUL);
            this->_con->win_refresh();
        } else if (ERR == button) {
            waitExpired = true;
        }

        gettimeofday(endTime, NULL);
        timeToWait -= TotalMilliseconds(endTime, startTime) / 100;
        timeToWait = timeToWait > 1 ? timeToWait : 1;
        halfdelay(timeToWait);
    }

    raw();
    this->_con->setScore1(GO);
    this->_con->setScore2(GO);
    this->_con->win_refresh();

    gettimeofday(startTime, NULL);

    while (0L == currentTimeP1 || 0L == currentTimeP2) {
        int key = getch();
        if ((this->_p1Switch == key) && 0L == currentTimeP1) {
            gettimeofday(endTime, NULL);
            currentTimeP1 = TotalMilliseconds(endTime, startTime);
            this->_con->setScore1(Score(currentTimeP1));
            this->_con->win_refresh();
        } else if ((_p2Switch == key) && 0L == currentTimeP2) {
            gettimeofday(endTime, NULL);
            currentTimeP2 = TotalMilliseconds(endTime, startTime);
            this->_con->setScore2(Score(currentTimeP2));
            this->_con->win_refresh();
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

void Game::OnePlayerGame() {
    raw();
    this->_con->setMarquee(ONE_PLAYER_GAME);
    this->_con->win_refresh();

    long long totalTime = 0L;

    this->_firstPlayerFoul = false;

    for (int i = 0; i < RUN_NUMBER; ++i) {
        this->_con->setMarquee(GetRunNumber(i, RUN_NUMBER));
        this->_con->win_refresh();

        long long currentTime = this->ReactionTimeSolo();
        totalTime += currentTime;
        this->_con->setScore1(Score(currentTime));
        this->_con->win_refresh();
        this->TimeConsumingSleep(2000);
    }

    if (false == this->_firstPlayerFoul)
        this->_lastScoreP1 = totalTime;
    else
        this->_lastScoreP1 = -1;

    this->_lastScoreP2 = -1;

    this->UpdateScores(totalTime, _firstPlayerFoul);
}

/*
 * Two player game.
 */

void Game::TwoPlayerGame() {
    raw();
    this->_con->setMarquee(TWO_PLAYER_GAME);
    this->_con->win_refresh();

    long long totalScore1 = 0L;
    long long totalScore2 = 0L;

    for (int i = 0; i < RUN_NUMBER; ++i) {
        this->_con->setMarquee(GetRunNumber(i, RUN_NUMBER));
        this->_con->win_refresh();

        long long currentScore1 = 0L;
        long long currentScore2 = 0L;
        this->ReactionTimeDuo(currentScore1, currentScore2);

        this->_con->setScore1(Score(currentScore1));
        this->_con->setScore2(Score(currentScore2));
        this->_con->win_refresh();

        totalScore1 += currentScore1;
        totalScore2 += currentScore2;
        this->TimeConsumingSleep(2000);
    }

    if (false == this->_firstPlayerFoul)
        this->_lastScoreP1 = totalScore1;
    else
        this->_lastScoreP1 = -1;

    if (false == this->_secondPlayerFoul)
        this->_lastScoreP2 = totalScore2;
    else
        this->_lastScoreP2 = -1;

    this->UpdateScores(totalScore1, _firstPlayerFoul);
    this->UpdateScores(totalScore2, _secondPlayerFoul);
}

void Game::AttractMode(short cycle) {
    switch (cycle) {
        case 0:
            // Last scores
            if (true == this->_firstPlayerFoul)
                this->_con->setScore1(NOSCORE);
            else if (this->_lastScoreP1 == -1)
                this->_con->setScore1("");
            else
                this->_con->setScore1(_lastScoreP1);

            if (true == this->_secondPlayerFoul)
                this->_con->setScore2(NOSCORE);
            else if (this->_lastScoreP2 == -1)
                this->_con->setScore2("");
            else
                this->_con->setScore2(_lastScoreP2);

            break;
        case 1:
            // Hish score
            this->_con->setScore1(HIGH_SCORE);
            this->_con->setScore2(_highScore);
            break;
        case 2:
            // Average score
            this->_con->setScore1(AVERAGE_SCORE);
            this->_con->setScore2(_averageScore);
            break;
        case 3:
            // Nb of played games
            this->_con->setScore1(NUMBER_OF_PLAYED_GAMES);
            this->_con->setScore2(_nbGamesPlayed);
            break;
        case 4:
            // Nb of fouled games
            this->_con->setScore1(FOULED);
            this->_con->setScore2(_nbGamesFouled);
            break;
        case 5:
            // Nb of amaizing games
            this->_con->setScore1(AMAIZING_GAMES);
            this->_con->setScore2(_amaizingScore);
            break;
    }
}

/*
 * Paying game.
 */

void Game::NonFreeGameLoop() {
    short cycle = 0;
    bool doStop = false;

    while (false == doStop) {
        this->_con->setMarquee(INSERT_COIN);

        this->AttractMode(cycle);

        this->_con->win_refresh();

        halfdelay(50);
        int button = getch();

        if (this->_creditKey == button) {
            this->_con->setMarquee(WAITING_FOR_PLAYER_2);
            this->_con->setScore1("");
            this->_con->setScore2("");
            this->_con->win_refresh();

            button = getch();

            if (this->_creditKey == button)
                this->TwoPlayerGame();
            else
                this->OnePlayerGame();

            cycle = 5;
        } else if (KEY_F(1) == button) {
            KeyConfig();
            cycle = 5;
        } else if (KEY_F(2) == button) {
            ShowConfig();
            cycle = 5;
        } else if (_quitKey == button) {
            doStop = true;
        }

        cycle = (cycle + 1) % 6;
    }
}

/*
 * Free game.
 */

void Game::FreeGameLoop() {
    short cycle = 0;
    bool doStop = false;

    while (false == doStop) {
        this->_con->setMarquee(PRESS_START);

        this->AttractMode(cycle);

        this->_con->win_refresh();

        halfdelay(50);
        int button = getch();

        if (this->_p1Start == button) {
            this->OnePlayerGame();
            cycle = 5;
        } else if (this->_p2Start == button) {
            this->TwoPlayerGame();
            cycle = 5;
        } else if (this->_quitKey == button) {
            doStop = true;
        } else if (KEY_F(1) == button) {
            this->KeyConfig();
            cycle = 5;
        } else if (KEY_F(2) == button) {
            this->ShowConfig();
            cycle = 5;
        }

        cycle = (cycle + 1) % 6;
    }
}

/**
 * Error when reading configuration.
 */

void Game::ErrorReadingConfigFile() {
    cerr << ERR_CONFIG_READ << endl;
    exit(EXIT_FAILURE);
}

/*
 * Get char from config file.
 */

unsigned char Game::GetConfigFileChar(ifstream &configFile) {
    string data;

    if (configFile.good())
        getline(configFile, data);
    else
        this->ErrorReadingConfigFile();

    return (unsigned char) data[0];
}

/*
 * get int from config file.
 */

int Game::GetConfigFileInt(ifstream &configFile) {
    string data;

    if (configFile.good())
        getline(configFile, data);
    else
        this->ErrorReadingConfigFile();

    return atoi(data.c_str());
}

/*
 * Writes default config file
 */

void Game::WriteConfigFile() {
    ofstream configFile(CONFIG_FILE);
    if (configFile.is_open()) {
        configFile << this->_p1Start << endl;
        configFile << this->_p2Start << endl;

        configFile << this->_p1Switch << endl;
        configFile << this->_p2Switch << endl;

        configFile << this->_creditKey << endl;

        configFile << this->_quitKey << endl;

        configFile << this->_highScore << endl;
        configFile << this->_nbGamesPlayed << endl;
        configFile << this->_averageScore << endl;
        configFile << this->_amaizingScore << endl;
        configFile << this->_nbGamesFouled << endl;

        configFile.close();
    } else {
        cerr << ERR_CONFIG_WRITE << endl;
        exit(EXIT_FAILURE);
    }
}

/*
 * Reads config file and set keys
 */

void Game::ReadConfigFile() {
    ifstream configFile(CONFIG_FILE);

    if (configFile.is_open()) {
        this->_p1Start = GetConfigFileChar(configFile);
        this->_p2Start = GetConfigFileChar(configFile);

        this->_p1Switch = GetConfigFileChar(configFile);
        this->_p2Switch = GetConfigFileChar(configFile);

        this->_creditKey = GetConfigFileChar(configFile);

        this->_quitKey = GetConfigFileChar(configFile);

        this->_highScore = GetConfigFileInt(configFile);
        this->_nbGamesPlayed = GetConfigFileInt(configFile);
        this->_averageScore = GetConfigFileInt(configFile);
        this->_amaizingScore = GetConfigFileInt(configFile);
        this->_nbGamesFouled = GetConfigFileInt(configFile);

        configFile.close();
    } else {
        this->WriteConfigFile();
    }
}
