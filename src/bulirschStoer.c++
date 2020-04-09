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
				error("{h} / {step} ("+std::to_string(h)+" / "+std::to_string(step)+
				 	") not greater than 0.0, cannot simulate.", __LINE__, __FILE__);
				return 1;
			}
		}
	}
}
std::vector<Force> BulirschStoer::gravity() {
	println(in("BulirschStoer", "gravity")+"    Calculating gravitational forces", 4);
	CBody* body;
	std::vector<Force> forces(nreal, Force(0.0, 0.0, 0.0));
	for (int ii = 0; ii < nreal; ii ++) {
		body = read[ii];
		if (read[ii] != body) {
			printrln("\n"+in("BulirschStoer", "gravity")+"    Target: ", body->Name(), 5); 

			forces[ii] += body->r.direction(read[ii]->r) * (G * body->Mass() * read[ii]->Mass()) / (read[ii]->squareDistance(body));

			printrln(in("BulirschStoer", "gravity")+"    Magnitude of force between "+body->Name()+" and "+
				read[ii]->Name()+" is ", scientific(G * body->Mass() * read[ii]->Mass()) / (read[ii]->squareDistance(body)), 5);
			printrln(in("BulirschStoer", "gravity")+"    Net force vector on "+body->Name()+" is ", body->net.info(), 5);
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


