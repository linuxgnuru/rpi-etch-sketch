/*
 * Raspberry Pi Etch-a-sketch
 *
 * John Wiggins
 * jcwiggi@gmail.com
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ncurses.h>

#include <wiringPi.h>

#define MY_8591 100

// colors for drawing with:
#define CURSOR_COLOR_FG COLOR_WHITE
#define CURSOR_COLOR_BG COLOR_GREEN

const int addr = 0x48;
const int tiltPin = 6;

// Debouncing stuff
int buttonState;
int lastButtonState;
long lastDebounceTime;
long debounceDelay = 50;

// my own function to mimic the map function from Arduino
double map(double x, double x0, double x1, double y0, double y1)
{
	double y = y0 + ((y1 - y0) * ((x - x0) / (x1 - x0)));
	return y;
}

int main(int argc, char **argv)
{
	int x, y;
	int ch;
	int maxx, maxy;
	int reading;

	wiringPiSetup();
	pcf8591Setup(MY_8591, addr);
	pinMode(tiltPin, INPUT);
	pullUpDnControl(tiltPin, PUD_UP);
	initscr();
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, CURSOR_COLOR_FG, CURSOR_COLOR_BG);
	noecho();
	cbreak();
	nodelay(stdscr, true);
	wclear(stdscr);
	attron(A_BOLD);
	attron(COLOR_PAIR(1));
	printw("Raspberry Pi Etch-A-Sketch");
	attroff(A_BOLD);
	attroff(COLOR_PAIR(1));
	mvaddstr(1, 0, "Press any key to quit");
	refresh();
	getmaxyx(stdscr, maxy, maxx);
	while (1)
	{
		// Debounce the tilt switch (is this needed?)
		reading = digitalRead(tiltPin);
		if (reading != lastButtonState)
		       	lastDebounceTime = millis(); // reset the debouncing timer
		if ((millis() - lastDebounceTime) > debounceDelay)
		{
			if (reading != buttonState)
			{
				buttonState = reading;
				if (buttonState == LOW)
					clear();
			}
		}
		lastButtonState = reading;
		y = map(analogRead(MY_8591), 0, 255, 0, maxy - 1);
		x = map(analogRead(MY_8591 + 1), 0, 255, 0, maxx - 1);
		move(y, x);
		attron(A_BOLD);
		attron(COLOR_PAIR(2));
		printw("*");
		attroff(COLOR_PAIR(2));
		attroff(A_BOLD);
		refresh();
		ch = getch();
		if (ch > -1)
			break;
	}
	endwin();
	return EXIT_SUCCESS;
}
