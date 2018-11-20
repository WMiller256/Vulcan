#ifndef SIMIO_H
#define SIMIO_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <ncurses.h>
#include <unistd.h>

#include "csim.h"
#include "hash.h"

Hash* readfile(const std::string& filename);
bool exists(const std::string& filename);
bool prompt(const std::string& p);

void print(const std::string& s, int depth = 1);
void print(const int& i, int depth = 1);
void print(const double& d, int depth = 1);
void print(const float& f, int depth = 1);
void println(const std::string& s, int depth = 1);
void println(const int& i, int depth = 1);
void println(const double& d, int depth = 1);
void println(const float& f, int depth = 1);

void printr(const std::string& l, const std::string r, int depth = 1);
void printrln(const std::string& l, const std::string r, int depth = 1);

int nchar(const std::string& s);

int winwidth();
int winheight();

void print_special(const std::string& str, const char& fore, const char& back);

#endif
