/*
 * William Miller
 * Nov 8, 2018
 * 
 * Implementation file for force vector data structure
 *
 *
 */

#include "force.h"

Force::Force() : vec() {
}
Force::Force(double X, double Y, double Z) : vec(X, Y, Z) {
}
