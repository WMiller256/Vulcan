/*
 * baseio.h
 *
 * William Miller
 * Nov 8, 2018
 *
 * Header file for basic IO operations
 *
 *
 *
 */

#ifndef BASEIO_H
#define BASEIO_H

#include <string>
#include <iostream>
#include <math.h>

#include "colors.h"

void error(const std::string& s, const int line, const char* file);
void warning(const std::string& s, const int line, const char* file);

std::string scientific(double d, int precision = 5);
std::string stripcolors(const std::string& str);

bool iscolor(std::string str);

#endif // BASEIO_H
