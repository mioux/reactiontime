/* 
 * File:   Console.cpp
 * Author: mioux
 * 
 * Created on 4 mars 2011, 23:09
 */

#include "Console.h"
#include "displaystrings.h"
#include <assert.h>

#define VERSION "Version 0.14"
#define MARQUEE_MAX_SIZE 16
#define TEXT_MAX_SIZE 6

#define SCREEN_X 18
#define SCREEN_Y 4

using namespace std;

/*
 * Constructor. Shows the content.
 */
Console::Console()
{
    int x_center = (COLS - SCREEN_X) / 2;
    int y_center = (LINES - SCREEN_Y) / 2;

    mvprintw(LINES - 1, 0, CONFIG_KEY);
    mvprintw(LINES - 1, COLS - strlen(VERSION), VERSION);
    refresh();

    this->_window = newwin(SCREEN_Y, SCREEN_X, y_center, x_center);
    /*wborder(this->_window, (char)186, (char)186, // ||
                           (char)205, (char)205, // =
                           (char)201, (char)187, // coins haut
                           (char)200, (char)188); //coins bas*/

    box(this->_window, 0 , 0);

    mvwprintw(this->_window, 2, 8, "||");

    this->setMarquee("XXXXXXXXXXXXXXXX");
    this->setScore1("XXXXXX");
    this->setScore2("XXXXXX");

    this->win_refresh();

    //this->show();
}

/*
 * Copy.
 */

Console::Console(const Console& orig)
{
    this->_marquee = orig._marquee;
    this->_score1 = orig._score1;
    this->_score2 = orig._score2;
}

/*
 * Destructor. Nothing to do too.
 */

Console::~Console()
{

}

/*
 * Format text to fit exactly X chacacters
 */

string Console::formatText(string text, short size)
{
    short maxSize = text.length();
    maxSize = maxSize < size ? maxSize : size;
    string data = text.substr(0, maxSize);
    for (int i = maxSize; i < size; ++i)
        data.append(" ");

    return data;
}

/*
 * Change marquee text.
 */

void Console::setMarquee(string marquee)
{
    this->_marquee = this->formatText(marquee, MARQUEE_MAX_SIZE);
    
    mvwprintw(this->_window, 1, 1, this->_marquee.c_str());
}

string Console::scoreToString(short score)
{
    ostringstream oss (ostringstream::out);
    if (score < 10)
        oss << "     " << score;
    else if (score < 100)
        oss << "    " << score;
    else if (score < 1000)
        oss << "   " << score;
    else
        oss << "  " << score;

    return oss.str();
}

/*
 * Change score 1 text.
 */

void Console::setScore1(string score1)
{
    this->_score1 = this->formatText(score1, TEXT_MAX_SIZE);

    mvwprintw(this->_window, 2, 1, this->_score1.c_str());
}

void Console::setScore1(short score1)
{
    this->setScore1(scoreToString(score1));
}

/*
 * Change score 2 text.
 */

void Console::setScore2(string score2)
{
    this->_score2 = this->formatText(score2, TEXT_MAX_SIZE);
    
    mvwprintw(this->_window, 2, 11, this->_score2.c_str());
}

void Console::setScore2(short score2)
{
    this->setScore2(scoreToString(score2));
}

/*
 * Display scores
 */

void Console::win_refresh()
{
    wrefresh(this->_window);
}
