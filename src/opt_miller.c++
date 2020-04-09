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

void Miller::main() {
	CBody body;
	CBody wbody;
	vec v;
	vec a;
	CBody b;
	double dt;
	for (int ii = 0; ii < nreal; ii ++) {
		body = *read[ii];
		wbody = *write[ii];
		dt = simTime - body.fix;
		if (dt >= body.h) {
			wbody.net.zero();
			for (int jj = 0; jj < nreal; jj ++) {
				if (*read[jj] != body) {
					wbody.net += body.r.direction(read[jj]->r) * (G * body.Mass() * read[jj]->Mass()) / read[jj]->squareDistance(body.r);

				}
			}
		    a = wbody.net / body.Mass() * dt;
		    v = wbody.accelerate(a);
		    wbody.r = body.r + (v + a * 0.5) * dt;
		    wbody.fix = simTime;
		    wbody.ncalcs++;
		}
	}
}
