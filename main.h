/* 
 * File:   main.h
 * Author: srumeu
 *
 * Created on 9 mars 2011, 13:42
 */

#ifndef MAIN_H
#define	MAIN_H

#include <iostream>

extern void writeDefaultConfigFile();
extern short score(long long realScore);
extern long long totalMilliseconds(const timeval * const start, const timeval * const end);
extern long long reactionTimeSolo();
extern void reactionTimeDuo(long long &currentTimeP1, long long &currentTimeP2);
extern void twoPlayerGame();
extern void onePlayerGame();
extern void freeGameLoop();
extern void nonFreeGameLoop();
extern void errorReadingConfigFile();
extern char getConfigFileChar(ifstream &configFile);
extern int getConfigFileInt(ifstream &configFile);
extern void writeConfigFile();
extern void readConfigFile();

#endif	/* MAIN_H */

