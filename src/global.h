/*
 * global.h
 * 
 * William Miller
 * Nov 8, 2018
 * 
 * Prototypes for 'globally' available functions
 *
 */

#ifndef GLOBAL_H
#define GLOBAL_H

#include <string>
#include <iostream>
#include <atomic>
#include <math.h>

#include "vec.h"
#include "simio.h"
#include "colors.h"

double magnitude(vec v);
double fetch_add(std::atomic<double>* shared, double h);

#endif // GLOBAL_H
