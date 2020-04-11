/* miller.c++
 *
 * William Miller
 * Apr 7, 2020
 *
 * Implementation of Miller integrator
 *
 */

#include "miller.h"

Miller::Miller() {
}

void Miller::main(CBody* body, CBody* wbody) {
	vec v;
	vec a;
	double dt;
	dt = simTime - body->fix;
	if (dt >= body->h) {
		wbody->net.zero();
		for (int jj = 0; jj < nreal; jj ++) {
			if (read[jj] != body) {
				wbody->net += body->r.direction(read[jj]->r) * (G * read[jj]->m) / read[jj]->squareDistance(body->r);

			}
		}
	    a = wbody->net * dt;
	    v = wbody->accelerate(a);
	    wbody->r = body->r + (v + a * 0.5) * dt;
	    wbody->fix = simTime;
	    wbody->ncalcs++;
	}
}
