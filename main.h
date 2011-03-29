/* 
 * File:   main.h
 * Author: srumeu
 *
 * Created on 9 mars 2011, 13:42
 */

#ifndef MAIN_H
#define	MAIN_H

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
#include <string>

#define FREE_GAME_SWITCH "--free-game"
#define MAX_SCORE 9999
#define CONFIG_FILE "rtime.cfg"
#define AMAIZING_SCORE 600
#define RUN_NUMBER 5

extern void attractMode(short cycle);
extern void errorReadingConfigFile();
extern void freeGameLoop();
extern unsigned char getConfigFileChar(ifstream &configFile);
extern int getConfigFileInt(ifstream &configFile);
extern int getRandomTimeToWait();
extern string getRunNumber(int currentRunNumber, int totalRunNumber);
extern void keyConfig();
extern void nonFreeGameLoop();
extern void onePlayerGame();
extern void reactionTimeDuo(long long &currentTimeP1, long long &currentTimeP2);
extern long long reactionTimeSolo();
extern void readConfigFile();
extern unsigned char redefineKey(string marquee);
extern void timeConsumingSleep(long long timeToWait);
extern short score(long long realScore);
extern long long totalMilliseconds(const timeval * const start, const timeval * const end);
extern void twoPlayerGame();
extern void updateScores(int score, bool foul);
extern void writeConfigFile();

#define BEEP() cout << '\a';

#endif	/* MAIN_H */

