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
				force();
			}
			else {
				error("{h} / {step} ("+std::to_string(h)+" / "+std::to_string(step)+
				 	") not greater than 0.0, cannot simulate.", __LINE__, __FILE__);
				return 1;
			}
		}
	}
}
void BulirschStoer::force() {
	return;
	CBody* body = read[0];
	Force net(0.0, 0.0, 0.0);
	double fmagnitude;
	double dist;
	for (auto b : read) {
		if (b != NULL && b->Type() != bodyType::ghost) {
			if (b != body) {
				printrln("\n"+in("BulirschStoer", "force")+"    Target: ", body->Name(), 5); 
				dist = b->distance(body);
				fmagnitude = (G * body->Mass() * b->Mass()) / (dist * dist);
				net += body->pos.direction(b->pos) * fmagnitude;

				printrln(in("BulirschStoer", "force")+"    Magnitude of force between "+body->Name()+" and "+
					b->Name()+" is ", scientific(fmagnitude), 4);
				printrln(in("BulirschStoer", "force")+"    Net force vector on "+body->Name()+" is ", body->net.info(), 4);
			}
		}
	}
}
int BulirschStoer::NSteps() {
	return nsteps;
}


