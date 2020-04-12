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
	wbody->a.zero();
	for (int ii = 0; ii < nreal; ii ++) {
		if (read[ii] != body) {

			wbody->a += body->r.direction(read[ii]->r) * (G * read[ii]->Mass()) / read[ii]->squareDistance(body->r);

		}
	}
    wbody->v = body->v + wbody->a * h;
    wbody->r = body->r + (body->v + wbody->a * h) * h;
    wbody->fix = simTime;
    wbody->ncalcs++;
}

void Integrator::set(int b, int g) {
	nbodies = b;
	nghosts = g;
	nreal = b + g;
}
