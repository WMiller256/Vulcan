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
	println(in("Miller", "main")+"    Calculating net forces", 4);
	vec v;
	vec a;
	double dt;
	dt = simTime - body->fix;
	if (dt >= body->h) {
		wbody->net.zero();
		for (int jj = 0; jj < nreal; jj ++) {
			if (read[jj] != body) {
				printrln("\n"+in("Miller", "main")+"    Target: ", body->Name(), 5); 
				wbody->net += body->r.direction(read[jj]->r) * (G * body->Mass() * read[jj]->Mass()) / read[jj]->squareDistance(body->r);

				printrln(in("Miller", "main")+"    Magnitude of force between "+body->Name()+" and "+
					read[jj]->Name()+" is ", scientific(G * body->Mass() * read[jj]->Mass() / read[jj]->squareDistance(body->r)), 5);
				printrln(in("Miller", "main")+"    Net force vector on "+body->Name()+" is ", body->net.info(), 5);
			}
		}
	    a = wbody->net / body->Mass() * dt;
	    v = wbody->accelerate(a);
	    wbody->r = body->r + (v + a * 0.5) * dt;
	    wbody->fix = simTime;
	    wbody->ncalcs++;
		println(in("Miller", "main")+green+"    Done"+res, 5);
	}
}
