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
	println(in("Miller", "main")+"          Calculating net force on "+bright+red+body->Name()+res, 4);
	vec v;
	vec a;
	double dt;
	dt = simTime - body->fix;
	if (dt >= body->h) {
		wbody->net.zero();
		for (int jj = 0; jj < nreal; jj ++) {
			if (read[jj] != body) {
				wbody->net += body->r.direction(read[jj]->r) * (G * body->Mass() * read[jj]->m) / read[jj]->squareDistance(body->r);

				printrln(in("Miller", "main")+"              Magnitude of force between "+body->Name()+" and "+
					read[jj]->Name()+" is ", scientific(G * body->Mass() * read[jj]->m / read[jj]->squareDistance(body->r)), 5);
			}
		}
	    a = wbody->net / body->Mass() * dt;
	    v = wbody->accelerate(a);
	    wbody->r = body->r + (v + a * 0.5) * dt;
	    wbody->fix = simTime;
	    wbody->ncalcs++;
		printrln(in("Miller", "main")+"          Net force vector on "+bright+red+body->Name()+res+" is ", wbody->net.info(), 5);
		println(in("Miller", "main")+green+"          Done"+res, 4);
	}
}
