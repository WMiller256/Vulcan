/*
 * vel.c++
 *
 * William Miller
 * Nov 8, 2018
 * 
 * Method definitions for (Vel) class, a structure which holds a three dimensional velocity vector
 *
 */

#include "vel.h"

Vel::Vel() : vec() {
}
Vel::Vel(double X, double Y, double Z) : vec(X, Y, Z) {
}

