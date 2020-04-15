/* mercury.c++
 *
 * William Miller
 * Apr 15, 2020
 *
 * Implementation of class derived from Integrator
 * base class. Intended to be a verbatim, or at least
 * verbatim-adjacent port of the original Mercury
 * hybrid symplectic integrator
 *
 */

#include "mercury.h"

Mercury::Mercury(double &h) : Integrator() {
	this->h = h;
}

void Mercury::bulirschStoer(CBody* b, CBody* w) {
	
}
