@echo off
SET PATH=D:\DJGPP\bin
gpp -g -Wall Console.cpp main.cpp Game.cpp -o rtime.exe -lpdcurses
pause
