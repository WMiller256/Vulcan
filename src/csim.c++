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

void CSim::addBody(CBody* body) {
	one.push_back(body);
	two.push_back(body);
	nadded++;
}
void CSim::addPlanet(CBody* body) {
	body -> Type(bodyType::planet);
	addBody(body);
}
void CSim::addGhost(CGhost* ghost) {
	ghost -> Type(bodyType::ghost);
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
		if (one[ii] -> Type() == bodyType::planet) {
			planets.push_back(one[ii]);
		}
		else if (one[ii] -> Type() == bodyType::ghost) {
			ghosts.push_back(one[ii]);
		}
		else {
			defs.push_back(one[ii]);
		}
	}
	std::vector<CBody*> bodies;
	nplanets = planets.size();
	nghosts = ghosts.size();
	ndefs = defs.size();
	for (int ii = 0; ii < nplanets; ii ++) {
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
		if (one[ii] -> Type() == bodyType::def) {
			ret++;
		}
	}
	return ret;
}
int CSim::NReal() {
	const int size = one.size();
	int ret = 0;
	for (int ii = 0; ii < size; ii ++) {
		if (one[ii] -> Type() != bodyType::def) {
			ret++;
		}
	}
	return ret;
}

int CSim::writeConfiguration(const std::string& filename, bool overwrite) {
	std::ofstream out;
	if (exists(filename) && overwrite == false) {
		if (!prompt("File "+yellow+filename+res+" exists, overwrite? (y|n) ")) {
			println("File will not be overwritten, exiting "+cyan+"CSim"+yellow+"::"+bright+white+"writeConfiguration"+res);
			return 0;
		}
		else {
			println("Overwriting "+yellow+filename+res);
		}
	}
	print("Writing file "+yellow+filename+res+"... ");
	out.open(filename);
	out << nadded - ndefs - 1 << "\n";
	CBody* current;
	print_special("Writestream", 'c', 'k');		// Exclude
	for (int ii = 0; ii < nadded; ii ++) {
		current = read[ii];
		if (current != NULL && current -> Type() != bodyType::def) {
			out << current -> writeFormat();
			std::cout << current -> writeFormat();
		}
		else {
			error("Body "+std::to_string(ii)+" was "+bright+red+"NULL"+res+" - skipping.", __LINE__, __FILE__);
		}
	}
	println(green+"done"+res);
	return 0;
}

CSim* CSim::readConfiguration(const std::string& filename) {
	bool check;
	if (!exists(filename)) {
		std::cout << check << std::endl;
		error("File "+yellow+filename+res+" does not exist.",__LINE__,__FILE__);
		return NULL;
	}
	print("Reading file "+yellow+filename+res+"... ");
	print("\n", 2);
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

	println("Number of bodies: "+bright+red+std::to_string(nbodies)+res, 3);
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
			println(line, 3);
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
				error(" Read error", __LINE__, __FILE__);
				return NULL;
			}
			std::getline(file, line); 
			println(line, 3);
			n++;
			radius = stof(line.substr(line.find_last_of(" ")+1));
			std::getline(file, line); 
			println(line, 3);
			n++;
			mass = stof(line.substr(line.find_last_of(" ")+1));

			CBody* body = new CBody(mass, radius, speed, x, y, z);
			body -> Name(name);
			
			print(body -> info(), 2);
		}
	}
	debug = gdebug;
	println(green+"done"+res);
	return sim;
}

void CSim::fixedHForce(CBody* body, CBody* wbody) {
	println(in("CSim", "fixedHForce")+"    Calculating net force", 4);
	Force net(0,0,0);
	vec v;
	vec a;
	double fmagnitude;
	for (int ii = 0; ii < nplanets; ii ++) {
		if (read[ii] != NULL) {
			if (read[ii] != body) {
				printrln("\n"+in("CSim", "fixedHForce")+"    Target: ", body -> Name(), 5); 
				fmagnitude = (G * body -> Mass() * read[ii] -> Mass()) / pow(read[ii] -> distance(body -> pos), 2);
				net += body -> pos.direction(read[ii] -> pos) * fmagnitude;

				printrln(in("CSim", "fixedHForce")+"    Magnitude of force between "+body -> Name()+" and "+
					read[ii] -> Name()+" is ", scientific(fmagnitude), 4);
				printrln(in("CSim", "fixedHForce")+"    Net force vector on "+body -> Name()+" is ", body -> net.info(), 4);
			}
		}
	}
    a = net / body -> Mass() * h;
    v = wbody -> accelerate(a);
    wbody -> Position(body -> pos + v + a * (h * 0.5));
    wbody -> fix = simTime;
    wbody -> ncalcs++;
	println(in("CSim", "fixedHForce")+green+"    Done"+res+" net force", 5);	
}

void CSim::sim(threadmode t) {
	auto start = std::chrono::high_resolution_clock::now();
	void (CSim::*f)(CBody*, CBody*);
	if (forces) {
		f = &CSim::fixedHForce;
		calcs.push_back(f);
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
			if (nthreads > std::thread::hardware_concurrency - 1) {		// If the specified number of threads is greater than the maximum 
				nthreads = std::thread::hardware_concurrency - 1;		// of the current machine adjust accordingly.
			}
			if (nbodies < nthreads) {
				nthreads = nbodies;
			}
			else {
				while (nadded % nthreads != 0) {
					CBody* body = new CBody(0, 0, 0, 0, 0, 0, 1);
					addBody(body);
				}
			}
			sort();
			nbodies = nadded;
			std::cout << " Bodies -     "+bright+magenta+std::to_string(nbodies)+res+"\n";
			std::cout << "     Real:    "+bright+magenta+std::to_string(NReal())+res+"\n";
			std::cout << "     Virtual: "+bright+magenta+std::to_string(NVirtual())+res+"\n";

			int block = nbodies / nthreads;
			std::thread threads[nthreads];
			for (int ii = 0; ii < nthreads - 1; ii ++) {
				threads[ii] = std::thread(&CSim::threadedFixedH, this, block * ii, block * (ii + 1));
			}
			threads[nthreads - 1] = std::thread(&CSim::threadedFixedH, this, block * (nthreads - 1), nbodies);
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
                    if (simTime / percent != past) {
                        std::cout << '\r' << " Progress: " << std::setw(3) << (simTime * 100) / ((unsigned long long)maxTime - (unsigned long long)h) << std::flush;
                        past = simTime / percent;
                    }
                    if (simTime == maxTime - h) {
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
					println(in("CSim","sim")+"                Sim time - "+std::to_string(simTime),1);
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
				error("{h} is less than or equal to 0.0, cannot simulate.", __LINE__, __FILE__);
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
	int ii = 0;
	CBody* body = NULL;
	CBody* wbody = NULL;
	if (max > NReal()) max = NReal();	// Neglect the virtual bodies (they exist only to create an optimal block width)
	while (simTime == 0) {}				// Wait for the simulation to start
	while (true) {
#ifdef profiling
		auto start = std::chrono::high_resolution_clock::now();
#endif
		if (simTime - prev > 0) {
#ifdef profiling
			polltime += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
			start = std::chrono::high_resolution_clock::now();
#endif
			prev = simTime;
			for (int ii = min; ii < max; ii ++) {
				for (int jj = 0; jj < ncalcs; jj ++) {
					(this->*(calcs[jj]))(read[ii], write[ii]);
				}

				print(in("CSim", "threadedFixedH")+"       Velocity of {"+cyan+wbody -> Name()+res+"} is "+wbody -> Velocity().info(3)+"\n", 1);
				print(in("CSim", "threadedFixedH")+"       New posiiton for {"+cyan+wbody -> Name()+res+"} is "+wbody -> pos.info(3)+"\n", 1);
			}
			tocalc--;
#ifdef profiling
			cputime += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
#endif
			if (simTime == maxTime) {
				break;
			}
		}
	}
	joinable++;
}
void CSim::unthreadedFixedH(unsigned long end) {
	printrln(in("", "unthreadedFixedH(CSim*, double)"), "Simulating", 5);
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
				printrln(in("", "unthreadedFixedH")+"       Velocity of {"+cyan+body -> Name()+res+"} is ", body -> Velocity().info(3), 1);
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
		error("{h} was less than or equal to zero. Cannot simulate",__LINE__,__FILE__);
		return;
	}
	printrln(in("", "unthreadedFixedH(CSim*, double)"), green+"Complete"+res, 5);
}

void CSim::init() {
	print("Initializing new "+cyan+bright+"CSim"+res+"...");
	forces = true;
	ncalcs = 0;
	
	tMax = 0.0;
	tCurr = 0.0;
	h = -1.0;
	nadded = 0;
	ndefs = 0;
	nghosts = 0;
	nplanets = 0;
	print(green+" done\n"+res);
}

