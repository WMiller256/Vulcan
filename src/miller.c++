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
	thread_local double dt = simTime - wbody->fix;
	// TODO Need to extrapolate stale positions along previous orbits to 
	// current time for use in acceleration calculation
	if (dt >= wbody->h) {
		println(in("Miller", "main")+"          Calculating gravitational acceleration on "+bright+red+wbody->Name()+res, 4);
		wbody->a.zero();
		for (int jj = 0; jj < nreal; jj ++) {
			if (write[jj] != wbody) {
				wbody->a += wbody->r.direction(write[jj]->r) * (G * write[jj]->m) / write[jj]->squareDistance(wbody->r);

				printrln(in("Miller", "main")+"              Magnitude of force between "+wbody->Name()+" and "+
					write[jj]->Name()+" is ", scientific(G * wbody->m * write[jj]->m / write[jj]->squareDistance(wbody->r)), 5);
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
		printrln(in("Miller", "main")+"          Acceleration vector on "+bright+red+wbody->Name()+res+" is ", wbody->a.info(), 4);
		println(in("Miller", "main")+green+"          Done"+res, 4);
	}
}
