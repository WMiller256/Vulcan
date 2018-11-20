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
#include <math.h>

#include "colors.h"
#include "vel.h"
#include "pos.h"
#include "baseio.h"
#include "simio.h"

std::string in(std::string prefix, std::string function);
double magnitude(vec vec);

#endif // GLOBAL_H
