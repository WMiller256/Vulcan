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

void Miller::main(CBody* body, CBody* wbody) {
	double dt = simTime - body->fix;
	if (dt >= body->h) {
		wbody->a.zero();
		for (int jj = 0; jj < nreal; jj ++) {
			if (read[jj] != body) {
				wbody->a += body->r.direction(read[jj]->r) * (G * read[jj]->m) / read[jj]->squareDistance(body->r);

			}
		}
		// Apply acceleration to velocity
		wbody->v = body->v + wbody->a * dt;
		body->v = wbody->v;
    	// Apply acceleration to position, extra factor of two comes from applying acceleration to endpoint as well 
	    // as initial position. This significantly mitigates temporal degeneracy of the integration accuracy
	    wbody->r = body->r + (body->v + wbody->a * dt) * dt;
	    body->r = wbody->r;
	    // Update the fix time of the body
	    wbody->fix = simTime;
	    body->fix = wbody->fix;
	    wbody->totSteps++;
	    body->totSteps = wbody->totSteps;
	}
}
