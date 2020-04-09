/* bulirschStoer.c++
 *
 * William Miller
 * Apr 7, 2020
 *
 * Implementation for Bulirsch Stoer integrator, 
 * inherited from Integrator base class
 *
 */

#include "bulirschStoer.h"

int BulirschStoer::nsteps = 12;
int BulirschStoer::steps[] = { 2,4,6,8,12,16,24,32,48,64,96,128 };

BulirschStoer::BulirschStoer(){
	init();
}

void BulirschStoer::init() {
	threshold = 100000000;
	rscale = std::vector<double>(nreal, 0.0);
}

int BulirschStoer::step() {
	Vel v;
	Pos c;
	vec f;
	Pos p = c;
	int hdid;
	for (auto step: steps) {
		for (auto body : read) {
			if (h / step > 0.0) {
				main();
			}
			else {
				return 1;
			}
		}
	}
}
std::vector<Force> BulirschStoer::gravity() {
	CBody* body;
	std::vector<Force> forces(nreal, Force(0.0, 0.0, 0.0));
	for (int ii = 0; ii < nreal; ii ++) {
		body = read[ii];
		if (read[ii] != body) {

			forces[ii] += body->r.direction(read[ii]->r) * (G * body->Mass() * read[ii]->Mass()) / (read[ii]->squareDistance(body));

		}
	}
	return forces;
}
void BulirschStoer::main() {
	for (int ii = 0; ii < nreal; ii ++) {
		rscale[ii] = 1.0 / (read[ii]->r.squared());
		vscale[ii] = 1.0 / (read[ii]->v.squared());
	}
}
int BulirschStoer::NSteps() {
	return nsteps;
}


