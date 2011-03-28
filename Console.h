/* 
 * File:   Console.h
 * Author: mioux
 *
 * Created on 4 mars 2011, 23:09
 */

#ifndef CONSOLE_H
#define	CONSOLE_H

#include <string>
#include <stdlib.h>

using namespace std;

class Console {
public:
    Console();
    Console(const Console& orig);
    virtual ~Console();
    void setMarquee(string marquee);
    void setScore1(string score1);
    void setScore2(string score2);
    void setScore1(short score1);
    void setScore2(short score2);
    void win_refresh();
private:
    string _marquee;
    string _score1;
    string _score2;
    void show();
    string scoreToString(short score);
    string formatText(string text, short size);
};

#endif	/* CONSOLE_H */

