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
	double dt;
	dt = simTime - body->fix;
	if (dt >= body->h) {
		wbody->a.zero();
		for (int jj = 0; jj < nreal; jj ++) {
			if (read[jj] != body) {
				wbody->a += body->r.direction(read[jj]->r) * (G * read[jj]->m) / read[jj]->squareDistance(body->r);

			}
		}
		wbody->v = body->v + wbody->a * dt;
	    wbody->r = body->r + (body->v + 0.5 * wbody->a * dt) * dt;
	    wbody->fix = simTime;
	    wbody->ncalcs++;
	}
}
