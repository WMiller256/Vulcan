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
double Integrator::h = -1.0;
int Integrator::nghosts = 0;
int Integrator::nbodies = 0;
int Integrator::nreal = 0;

Integrator::Integrator() {}

void Integrator::main(CBody* body, CBody* wbody) {
	println(in("Integrator", "main")+"    Calculating net forces", 4);
	for (int ii = 0; ii < nbodies; ii ++) {
		if (read[ii] != body) {
			printrln("\n"+in("Integrator", "main")+"    Target: ", body->Name(), 5); 

			body->net += body->r.direction(read[ii]->r) * (G * body->Mass() * read[ii]->Mass()) / read[ii]->squareDistance(body->r);

			printrln(in("Integrator", "main")+"    Magnitude of force between "+body->Name()+" and "+
				read[ii]->Name()+" is ", scientific(G * body->Mass() * read[ii]->Mass() / read[ii]->squareDistance(body->r)), 4);
			printrln(in("Integrator", "main")+"    Net force vector on "+body->Name()+" is ", body->net.info(), 4);
		}
	}
    body->a = body->net / body->Mass() * h;
    body->v = static_cast<Vel>(wbody->accelerate(body->a));
    wbody->r += (body->v + body->a * 0.5) * h;
    wbody->fix = simTime;
    wbody->ncalcs++;
}

void Integrator::set(int b, int g) {
	nbodies = b;
	nghosts = g;
	nreal = b + g;
}
