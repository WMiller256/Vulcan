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
	// Apply acceleration to velocity
    wbody->v = body->v + wbody->a * h;
    // Apply acceleration to position, extra factor of two comes from applying acceleration to endpoint as well 
    // as initial position. This significantly mitigates temporal degeneracy of the integration accuracy
    wbody->r = body->r + wbody->v * h;
    // Update the fix time of the body
    wbody->fix = simTime;
    wbody->ncalcs++;

}

void Integrator::set(int b, int g) {
	nbodies = b;
	nghosts = g;
	nreal = b + g;
}

