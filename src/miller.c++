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

void Miller::force() {
	println(in("Miller", "force")+"    Calculating net force", 4);
	CBody* body;
	CBody* wbody;
	Force net(0,0,0);
	vec v;
	vec a;
	CBody b;
	for (int ii = 0; ii < nreal; ii ++) {
		body = read[ii];
		wbody = write[ii];
		if (simTime - body->fix >= body->h) {
			for (int jj = 0; jj < nreal; jj ++) {
				if (read[jj] != body) {
					printrln("\n"+in("Miller", "force")+"    Target: ", body->Name(), 5); 
					net += body->pos.direction(read[jj]->pos) * (G * body->Mass() * read[jj]->Mass()) / read[jj]->squareDistance(body->pos);

					printrln(in("Miller", "force")+"    Magnitude of force between "+body->Name()+" and "+
						read[jj]->Name()+" is ", scientific(G * body->Mass() * read[jj]->Mass() / read[jj]->squareDistance(body->pos)), 4);
					printrln(in("Miller", "force")+"    Net force vector on "+body->Name()+" is ", body->net.info(), 4);
				}
			}
		    a = net / body->Mass() * body->h;
		    v = wbody->accelerate(a);
		    wbody->Position(body->pos + v + a * body->h * 0.5);
		    wbody->fix = simTime;
		    wbody->ncalcs++;
		}
		println(in("Miller", "force")+green+"    Done"+res+" net force", 5);
	}
}
