/* 
 * File:   Game.h
 * Author: srumeu
 *
 * Created on 30 mars 2011, 13:00
 */

#ifndef GAME_H
#define	GAME_H

#include "Console.h"
#include "main.h"

using namespace std;

class Game {
private:
    bool _firstPlayerFoul;
    bool _secondPlayerFoul;

    unsigned char _p1Start;
    unsigned char _p2Start;

    unsigned char _creditKey;

    unsigned char _p1Switch;
    unsigned char _p2Switch;

    unsigned char _quitKey;

    int _nbGamesPlayed;
    int _highScore;
    int _averageScore;
    int _amaizingScore;
    int _nbGamesFouled;

    short _lastScoreP1;
    short _lastScoreP2;

    Console *_con;

    void AttractMode(short cycle);
    void ErrorReadingConfigFile();
    unsigned char GetConfigFileChar(ifstream &configFile);
    int GetConfigFileInt(ifstream &configFile);
    int GetRandomTimeToWait();
    string GetRunNumber(int currentRunNumber, int totalRunNumber);
    void KeyConfig();
    void OnePlayerGame();
    void ReactionTimeDuo(long long &currentTimeP1, long long &currentTimeP2);
    long long ReactionTimeSolo();
    void ReadConfigFile();
    unsigned char RedefineKey(string marquee);
    void TimeConsumingSleep(long long timeToWait);
    short Score(long long realScore);
    void ShowConfig();
    void ShowKeys(string marquee, unsigned char button1, unsigned char button2);
    long long TotalMilliseconds(const timeval * const start, const timeval * const end);
    void TwoPlayerGame();
    void UpdateScores(int score, bool foul);
    void WriteConfigFile();

public:
    Game();
    virtual ~Game();

    void FreeGameLoop();
    void NonFreeGameLoop();
};

#endif	/* GAME_H */

