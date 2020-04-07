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
				force(body);
			}
			else {
				return 1;
			}
		}
	}
}
vec BulirschStoer::force(CBody* body) {
	Force net(0.0, 0.0, 0.0);
	double fmagnitude;
	double dist;
	for (auto b : read) {
		if (b != NULL && b->Type() != bodyType::ghost) {
			if (b != body) {
				dist = b->distance(body);
				fmagnitude = (G * body->Mass() * b->Mass()) / (dist * dist);
				net += body->pos.direction(b->pos) * fmagnitude;

			}
		}
	}
	return net;
}
int BulirschStoer::NSteps() {
	return nsteps;
}


