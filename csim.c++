#include "csim.h"
#include "simio.h"

bool warnings = true;
int debug = 4;
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

void CSim::addBody(CBody* body) {
	bodies -> addNode(body);
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
