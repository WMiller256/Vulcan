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
	println(in("Miller", "main")+"    Calculating net forces", 4);
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
					printrln("\n"+in("Miller", "main")+"    Target: ", body.Name(), 5); 
					wbody.net += body.r.direction(read[jj]->r) * (G * body.Mass() * read[jj]->Mass()) / read[jj]->squareDistance(body.r);

					printrln(in("Miller", "main")+"    Magnitude of force between "+body.Name()+" and "+
						read[jj]->Name()+" is ", scientific(G * body.Mass() * read[jj]->Mass() / read[jj]->squareDistance(body.r)), 4);
					printrln(in("Miller", "main")+"    Net force vector on "+body.Name()+" is ", body.net.info(), 4);
				}
			}
		    a = wbody.net / body.Mass() * dt;
		    v = wbody.accelerate(a);
		    wbody.r = body.r + (v + a * 0.5) * dt;
		    wbody.fix = simTime;
		    wbody.ncalcs++;
		}
		println(in("Miller", "main")+green+"    Done"+res, 5);
	}
}
