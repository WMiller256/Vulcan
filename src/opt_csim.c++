#include "csim.h"
#include "simio.h"

bool warnings = true;
int nthreads;
int debug = 0;
int nbodies;
unsigned long long cputime = 0;
unsigned long long polltime = 0;
unsigned long long maxTime = 0;
unsigned long long simulationtime = 0;
#if SIMTIME_TYPE == INT
std::atomic<unsigned long long> simTime;
#elif SIMTIME_TYPE == DOUBLE
std::atomic<double> simTime;
#endif
std::atomic<int> tocalc;
std::atomic<int> joinable;

int CSim::BulirschStoer::nsteps = 12;
int CSim::BulirschStoer::steps[] = { 2,4,6,8,12,16,24,32,48,64,96,128 };

CSim::CSim() {
	init();	
	read = one;
	write = two;
}
CSim::CSim(int n) {
	init();
	nbodies = n;
	read = one;
	write = two;
}
CSim::CSim(int n, double max, double step) {
	init();
	nadded = 0;
	nbodies = n;
	h = step;
	tMax = max;
	maxTime = max;
	read = one;
	write = two;
}
CSim::~CSim() {
}

void CSim::setDebug(int Debug) {
	debug = Debug;
}
void CSim::Type(simType t) {
	type = t;
}
simType CSim::Type() {
	return type;
}

void CSim::addBody(CBody* body) {
	one.push_back(body);
	two.push_back(body);
	nadded++;
}
void CSim::addPlanet(CBody* body) {
	body->Type(bodyType::planet);
	addBody(body);
}
void CSim::addGhost(CGhost* ghost) {
	ghost->Type(bodyType::ghost);
	one.push_back(ghost);
	two.push_back(ghost);
	nadded++;
}
CBody* CSim::at(int ii) {
	return read[ii];
}
CBody CSim::copy(int ii) {
	return *read[ii];
}
double CSim::H() {
	return h;
}
int CSim::count() {
	return nadded;
}
void CSim::sort() {
	std::vector<CBody*> planets;
	std::vector<CBody*> ghosts;
	std::vector<CBody*> defs;
	for (int ii = 0; ii < nadded; ii++) {
		if (one[ii]->Type() == bodyType::planet) {
			planets.push_back(one[ii]);
		}
		else if (one[ii]->Type() == bodyType::ghost) {
			ghosts.push_back(one[ii]);
		}
		else {
			defs.push_back(one[ii]);
		}
	}
	std::vector<CBody*> bodies;
	nreal = planets.size();
	nghosts = ghosts.size();
	ndefs = defs.size();
	for (int ii = 0; ii < nreal; ii ++) {
		bodies.push_back(planets[ii]);
	}
	for (int ii = 0; ii < nghosts; ii ++) {
		bodies.push_back(ghosts[ii]);
	}
	for (int ii = 0; ii < ndefs; ii ++) {
		bodies.push_back(defs[ii]);
	}
	one = bodies;
	two = bodies;
	read = one;
	write = two;
}

int CSim::NVirtual() {
	int size = one.size();
	int ret = 0;
	for (int ii = 0; ii < size; ii ++) {
		if (one[ii]->Type() == bodyType::def) {
			ret++;
		}
	}
	return ret;
}
int CSim::NReal() {
	const int size = one.size();
	int ret = 0;
	for (int ii = 0; ii < size; ii ++) {
		if (one[ii]->Type() != bodyType::def) {
			ret++;
		}
	}
	return ret;
}

int CSim::writeConfiguration(const std::string& filename, bool overwrite) {
	std::ofstream out;
	if (exists(filename) && overwrite == false) {
		if (!prompt("File "+yellow+filename+res+" exists, overwrite? (y|n) ")) {
			return 0;
		}
		else {
		}
	}
	out.open(filename);
	out << nadded - ndefs - 1 << "\n";
	CBody* current;
	print_special("Writestream", 'c', 'k');		// Exclude
	for (int ii = 0; ii < nadded; ii ++) {
		current = read[ii];
		if (current != NULL && current->Type() != bodyType::def) {
			out << current->writeFormat();
			std::cout << current->writeFormat();
		}
		else {
		}
	}
	return 0;
}

CSim* CSim::readConfiguration(const std::string& filename) {
	bool check;
	if (!exists(filename)) {
		std::cout << check << std::endl;
		return NULL;
	}
	int gdebug = debug;
	if (debug < 3) {
		debug = 0;
	}
	std::ifstream file;
	std::string line;
	int n;
	file.open(filename);

	std::getline(file, line); 
	n++;
 	int nbodies = stof(line);
	CSim* sim = new CSim(nbodies);

	while (std::getline(file, line)) {
		n++;
		if (line.substr(0, 4) == "Body") {
			double x;
			double y;
			double z;
			double radius;
			double mass;
			double speed;

			std::string name;
			if (line.length() > 4) {
				name = line.substr(line.find(" ")+1);
			}
			else {
				name = "";
			}
			
			std::getline(file, line); 
			n++;
			std::size_t start = line.find(" ");
			if (start != std::string::npos) {
				z = stof(line.substr(line.find_last_of(" ")+1));
				line = line.substr(0, line.find_last_of(" "));
				y = stof(line.substr(line.find_last_of(" ")+1));
				line = line.substr(0, line.find_last_of(" "));
				z = stof(line.substr(line.find_last_of(" ")+1));
				line = line.substr(0, line.find_last_of(" "));
			}
			else {
				return NULL;
			}
			std::getline(file, line); 
			n++;
			radius = stof(line.substr(line.find_last_of(" ")+1));
			std::getline(file, line); 
			n++;
			mass = stof(line.substr(line.find_last_of(" ")+1));

			CBody* body = new CBody(mass, radius, speed, x, y, z);
			body->Name(name);
			
		}
	}
	debug = gdebug;
	return sim;
}

void CSim::fixedHForce(CBody* body, CBody* wbody) {
	Force net(0,0,0);
	vec v;
	vec a;
	double fmagnitude;
	for (int ii = 0; ii < nreal; ii ++) {
		if (read[ii] != NULL) {
			if (read[ii] != body) {
				fmagnitude = (G * body->Mass() * read[ii]->Mass()) / read[ii]->squareDistance(body->pos);
				net += body->pos.direction(read[ii]->pos) * fmagnitude;

			}
		}
	}
    a = net / body->Mass() * h;
    v = wbody->accelerate(a);
    wbody->Position(body->pos + v + a * h * 0.5);
    wbody->fix = simTime;
    wbody->ncalcs++;
}

void CSim::sim(threadmode t) {
	auto start = std::chrono::high_resolution_clock::now();
	CBody* b;
	CBody* w;
	if (forces) {
		if (type == simType::basic) {
			calcs.push_back(&CSim::fixedHForce);
		}
		else if (type == simType::miller) {
			calcs.push_back(&CSim::Miller::force);
		}
	}
	ncalcs = calcs.size();
	sort();
	switch(t) {
		case single:
#ifdef profiling
			start = std::chrono::high_resolution_clock::now();
#endif
			unthreadedFixedH((unsigned long)tMax);
#ifdef profiling
			simulationtime += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
#endif
			break;
		case manual:
			simTime = 0;
			tocalc = 0;
			joinable = 0;
			if (nthreads < 1) {
				nthreads = 1;
			}
			if (nthreads > std::thread::hardware_concurrency() - 1) {		// If the specified number of threads is greater than the maximum 
				nthreads = std::thread::hardware_concurrency() - 1;			// of the current machine adjust accordingly.
			}
			if (nadded < nthreads) {
				nthreads = nadded;
			}
			else {
				while (nadded % nthreads != 0) {
					CBody* body = new CBody(0, 0, 0, 0, 0, 0, 1);
					addBody(body);
				}
			}
			if (type == simType::bulirschStoer) {
			}
			sort();
			nbodies = nadded;

			int block = nbodies / nthreads;
//			cpu_set_t set;
//			CPU_ZERO(&set);
			std::thread threads[nthreads];
			for (int ii = 0; ii < nthreads - 1; ii ++) {
				threads[ii] = std::thread(&CSim::threadedFixedH, this, block * ii, block * (ii + 1));
//				CPU_SET(ii, &set);
//				pthread_setaffinity_np(threads[ii].native_handle(), sizeof(cpu_set_t), &set);
			}
			threads[nthreads - 1] = std::thread(&CSim::threadedFixedH, this, block * (nthreads - 1), nbodies);
//			CPU_SET(nthreads - 1, &set);
//			pthread_setaffinity_np(threads[nthreads - 1].native_handle(), sizeof(cpu_set_t), &set);
#ifdef profiling
			start = std::chrono::high_resolution_clock::now();
#endif
			if (int(h) > 0) {
				vec a;
				vec v;
				unsigned long long percent;
				int past = 0;
				if (maxTime > 100 * h) {
					percent = (maxTime - h) / 100;
				}
				else {
					percent = h;
				}
				while (simTime < maxTime) {
					tocalc = nthreads;
#if SIMTIME_TYPE == 1
                    if (simTime / percent != past && simTime < maxTime) {
                        std::cout << '\r' << " Progress: " << std::setw(3) << (simTime * 100) / ((unsigned long long)maxTime - (unsigned long long)h) << std::flush;
                        past = simTime / percent;
                    }
                    if (simTime >= maxTime - h) {
                        std::cout << "\r Progress: 100\n";
                    }
                    simTime += h;
#elif SIMTIME_TYPE == 2
                    if ((unsigned long long)simTime / percent != past) {
                        std::cout << '\r' << " Progress: " << std::setw(3) << ((unsigned long long)simTime * 100) / ((unsigned long long)maxTime - (unsigned long long)h) << std::flush;
                        past = (unsigned long long)simTime / percent;
                    }
                    if ((unsigned long long)maxTime - (unsigned long long)simTime <= h) {
                        std::cout << "\r Progress: 100\n";
                    }
                    fetch_add(&simTime, h);
#endif
					while (tocalc > 0) {}					
					if (read == one) {
						read = two;
						write = one;
					}
					else {
						read = one;
						write = two;
					}
				}
			}
			else {
			}
#ifdef profiling
			simulationtime += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
#endif
			while (joinable < nthreads - 1) {}
			for (int ii = 0; ii < nthreads; ii ++) {
				threads[ii].join();
			}
			break;
	}
}
void CSim::threadedFixedH(int min, int max) {
	int tot = 0;
	double prev = 0;
	double elapsed = 0;
	int ii = 0;
	if (max > NReal()) max = NReal();	// Neglect the virtual bodies (they exist only to create an optimal block width)
	while (simTime == 0) {}				// Wait for the simulation to start
	while (true) {
#ifdef profiling
		auto start = std::chrono::high_resolution_clock::now();
#endif
		elapsed = simTime - prev;
		if (elapsed > 0) {
#ifdef profiling
			polltime += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
			start = std::chrono::high_resolution_clock::now();
#endif
			if (simTime >= maxTime) {
				tocalc--;
				break;
			}
			prev = simTime;
			if (type == simType::basic) {
				for (int ii = min; ii < max; ii ++) {
					for (int jj = 0; jj < ncalcs; jj ++) {
						calcs[jj](read[ii], write[ii]);
					}

				}
			}
			else if (type == simType::bulirschStoer) {
				for (int ii = min; ii < max; ii ++) {
					BS->step();
				}
			}
			else if (type == simType::miller) {
				for (int ii = min; ii < max; ii ++) {
					for (int jj = 0; jj < ncalcs; jj ++) {
						calcs[jj](read[ii], write[ii]);
					}
				}
			}
			tocalc--;
#ifdef profiling
			cputime += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
#endif
		}
	}
	joinable++;
}
void CSim::unthreadedFixedH(unsigned long end) {
	unsigned long t = 0.0;
	double h = H();
	nbodies = count();
	int past = 0;
	CBody* body;
	vec a[nbodies];
	vec v[nbodies];
	if (h > 0.0) {
		unsigned long percent;
		if (end > 100 * h) {
			percent = (end - 1) / 100;
		}
		else {
			percent = h;
		}
		while (t < end) {
			if ((unsigned long)t / percent != past) {
				std::cout << '\r' << " Progress: " << std::setw(3) << ((unsigned long)t * 100) / (unsigned long)(end - h) << std::flush;
				past = (unsigned long)t / percent;
			}
#ifdef profiling
				auto start = std::chrono::high_resolution_clock::now();
#endif
			for (int ii = 0; ii < nbodies; ii ++) {
				fixedHForce(read[ii], write[ii]);
			}
			if (read == one) {
				read = two;
				write = one;
			}
			else {
				read = one;
				write = two;
			}
#ifdef profiling
			cputime += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
#endif
			t += h;
		}
		std::cout << "\r Progress: 100\n";
	}
	else {
		return;
	}
}

CSim::BulirschStoer::BulirschStoer(CSim* sim) {
	this->sim = sim;
	init();
}

void CSim::BulirschStoer::init() {
	threshold = 100000000;
	for (int ii = 0; ii < sim->nadded; ii ++) {
		sim->read[ii]->BS->positions.reserve(nsteps);
		sim->write[ii]->BS->positions.reserve(nsteps);
		for (int jj = 0; jj < nsteps; jj ++) {
			Pos p;
			sim->read[ii]->BS->positions.push_back(p);
			sim->write[ii]->BS->positions.push_back(p);
		}
	}
}

int CSim::BulirschStoer::step() {
	Vel v;
	Pos c;
	vec f;
	Pos p = c;
	int hdid;
	for (auto step: steps) {
		for (auto body : sim->read) {
			if (sim->h / step > 0.0) {
				force(body);
			}
			else {
				return 1;
			}
		}
	}
}
vec CSim::BulirschStoer::force(CBody* body) {
	Force net(0.0, 0.0, 0.0);
	double fmagnitude;
	double dist;
	for (auto b : sim->read) {
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
int CSim::BulirschStoer::NSteps() {
	return nsteps;
}

CSim::Miller::Miller(CSim* sim) {
	this->sim = sim;
}
void CSim::Miller::force(CBody* body, CBody* wbody) {
	if (simTime - body->fix >= body->h) {
		Force net(0,0,0);
		vec v;
		vec a;
		CBody b;
		for (int ii = 0; ii < sim->nreal; ii ++) {
			b = *sim->read[ii];
			if (b != *body) {
				net += body->pos.direction(b.pos) * (G * body->Mass() * b.Mass()) / b.squareDistance(body->pos);

			}
		}
	    a = net / body->Mass() * body->h;
	    v = wbody->accelerate(a);
	    wbody->Position(body->pos + v + a * body->h * 0.5);
	    wbody->fix = simTime;
	    wbody->ncalcs++;
	}
}

void CSim::init() {

	BS = new BulirschStoer(this);
	miller = new Miller(this);
	
	type = simType::basic;
	forces = true;
	ncalcs = 0;
	
	tMax = 0.0;
	tCurr = 0.0;
	h = -1.0;
	nadded = 0;
	ndefs = 0;
	nghosts = 0;
	nreal = 0;
}

