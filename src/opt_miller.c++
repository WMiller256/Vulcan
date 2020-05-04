/* miller.c++
 *
 * William Miller
 * Apr 7, 2020
 *
 * Implementation of Miller integration scheme, which supports the use of 
 * different time steps for different bodies. Intended to feature dynamic 
 * scaling of step size for each body.
 */

#include "miller.h"

Miller::Miller() {}

void Miller::basic(CBody* body, CBody* wbody) {
	double dt = simTime - wbody->fix;
	// TODO Need to extrapolate stale positions along previous orbits to 
	// current time for use in acceleration calculation
	if (dt >= wbody->h) {
		wbody->a.zero();
		for (int jj = 0; jj < nreal; jj ++) {
			if (write[jj] != wbody) {
				wbody->a += wbody->r.direction(write[jj]->r) * (G * write[jj]->m) / write[jj]->squareDistance(wbody->r);

			}
		}
		// Apply acceleration to velocity
		wbody->v = wbody->v + wbody->a * dt;
    	// Apply acceleration to position, extra factor of two comes from applying acceleration to endpoint as well 
	    // as initial position. This significantly mitigates temporal degeneracy of the integration accuracy
	    wbody->r = wbody->r + wbody->v * dt;
	    // Update the fix time of the body
	    wbody->fix = simTime;
	    wbody->totSteps++;
	}
}
