/* 
 * File:   Console.cpp
 * Author: mioux
 * 
 * Created on 4 mars 2011, 23:09
 */

#include "Console.h"
#include <iostream>
#include <sstream>

#define VERSION "               V 0.8"
#define MARQUEE_MAX_SIZE 16
#define TEXT_MAX_SIZE 6

using namespace std;

/*
 * Constructor. Shows the content.
 */
Console::Console()
{
    this->setMarquee("");
    this->setScore1("");
    this->setScore2("");

    this->show();
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
    
    this->show();
}

string Console::scoreToString(short score)
{
    ostringstream oss (ostringstream::out);
    if (score < 10)
        oss << "    " << score;
    else if (score < 100)
        oss << "   " << score;
    else if (score < 1000)
        oss << "  " << score;
    else
        oss << " " << score;

    return oss.str();
}

/*
 * Change score 1 text.
 */

void Console::setScore1(string score1)
{
    this->_score1 = this->formatText(score1, TEXT_MAX_SIZE);
    this->show();
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
    this->show();
}

void Console::setScore2(short score2)
{
    this->setScore2(scoreToString(score2));
}

/*
 * Display scores
 */

void Console::show()
{
    system("cls");
    cout << "+------------------+" << endl;
    cout << "| " << this->_marquee << " |" << endl;
    cout << "| " << this->_score1 << " || " << this->_score2 << " |" << endl;
    cout << "+--------++--------+" << endl;
    cout << VERSION << endl;

}
