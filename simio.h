#ifndef SIMIO_H
#define SIMIO_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#include "csim.h"
#include "hash.h"

Hash* read(const std::string& filename);
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

void error(const std::string& s, const int line, const char* file);

#endif
