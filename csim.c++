#include "csim.h"
#include "simio.h"

bool warnings = true;
int debug = 0;
int nbodies = 9;
double minradius = std::numeric_limits<double>::infinity();
double maxradius = 0;
double blockwidth;

CSim::CSim() {
	init();
	bodies = new Hash(nbodies);
}
CSim::CSim(int n) {
	init();
	nbodies = n;
	bodies = new Hash(n);
}
CSim::CSim(int n, double max) {
	init();
	nbodies = n;
	bodies = new Hash(n);
	tMax = max;
}
CSim::~CSim() {
}

void CSim::setDebug(int Debug) {
	debug = Debug;
}

void CSim::addBody(CBody* body) {
	bodies -> addNode(body);
}
void CSim::step() {
	bodies -> step();
}

void CSim::printForces() {
	bodies -> printForces();
}
void CSim::writeConfiguration(const std::string& filename) {
	bodies -> write(filename);
}
void CSim::readConfiguration(const std::string& filename) {
	bodies = read(filename);
}

void CSim::init() {
	print("Initializing new "+cyan+bright+"CSim"+res+"...");
	tMax = 0;
	tCurr = 0;
	print(green+" done\n"+res);
}
