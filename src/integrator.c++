/* integrator.c++
 *
 * William Miller
 * Apr 7, 2020
 *
 * Implementation of integrator base class
 *
 */

#include "csim.h"

std::vector<CBody*> Integrator::one;
std::vector<CBody*> Integrator::two;
std::vector<CBody*> Integrator::read;
std::vector<CBody*> Integrator::write;
double Integrator::h;
int Integrator::nadded;
int Integrator::ndefs;
int Integrator::nghosts;
int Integrator::nreal;

Integrator::Integrator() {
	h = -1.0;
	nadded = 0;
	ndefs = 0;
	nghosts = 0;
	nreal = 0;
}

void Integrator::main() {
	println(in("Integrator", "main")+"    Calculating net forces", 4);
	Force net(0,0,0);
	CBody* body;
	CBody* wbody;
	vec v;
	vec a;
	double fmagnitude;
	for (int ii = 0; ii < nreal; ii ++) {
		body = read[ii];
		wbody = write[ii];
		for (int jj = 0; jj < nreal; jj ++) {
			if (read[ii] != body) {
				printrln("\n"+in("Integrator", "main")+"    Target: ", body->Name(), 5); 

				net += body->r.direction(read[ii]->r) * (G * body->Mass() * read[ii]->Mass()) / read[ii]->squareDistance(body->r);

				printrln(in("Integrator", "main")+"    Magnitude of force between "+body->Name()+" and "+
					read[ii]->Name()+" is ", scientific(G * body->Mass() * read[ii]->Mass() / read[ii]->squareDistance(body->r)), 4);
				printrln(in("Integrator", "main")+"    Net force vector on "+body->Name()+" is ", body->net.info(), 4);
			}
		}
	    a = net / body->Mass() * h;
	    v = wbody->accelerate(a);
	    wbody->r = body->r + v + a * h * 0.5;
	    wbody->fix = simTime;
	    wbody->ncalcs++;
	}
}

