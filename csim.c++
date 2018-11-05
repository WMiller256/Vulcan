#include "csim.h"

bool warnings = true;
int nbodies = 9;
double minradius = std::numeric_limits<double>::infinity();
double maxradius = 0;

CSim::CSim() {
	init();
	bodies = new Hash(nbodies);
}
CSim::CSim(int n) {
	init();
	bodies = new Hash(n);
}
CSim::CSim(int n, double max) {
	init();
	bodies = new Hash(n);
	tMax = max;
}
CSim::~CSim() {
	delete bodies;
}

void CSim::init() {
	tMax = 0;
	tCurr = 0;
}
