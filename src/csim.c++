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

std::mutex mtx;

CSim::CSim() {
	init();	
	integrator->read = integrator->one;
	integrator->write = integrator->two;
}
CSim::CSim(int n) {
	init();
	nbodies = n;
	integrator->read = integrator->one;
	integrator->write = integrator->two;
}
CSim::CSim(int n, double max, double step) {
	init();
	nadded = 0;
	nbodies = n;
	integrator->h = step;
	maxTime = max;
	integrator->read = integrator->one;
	integrator->write = integrator->two;
}
CSim::~CSim() {}

void CSim::setDebug(int Debug) { debug = Debug;} 
void CSim::Type(simType t) { type = t; }
simType CSim::Type() { return type; }

void CSim::addBody(CBody* body) {
	integrator->one.push_back(body);
	// Force copy construction to create separate copy of 
	// body at different memory location - one set of bodies
	// is for reading from, the other is for writing to 
	// and they toggle every step
	CBody* wbody = new CBody(*body);
	integrator->two.push_back(wbody);
	nadded++;
}
void CSim::addPlanet(CBody* body) {
	body->Type(bodyType::planet);
	addBody(body);
}
void CSim::addGhost(CGhost* ghost) {
	ghost->Type(bodyType::ghost);
	integrator->one.push_back(ghost);
	// Force copy construction, see comment in CSim::addBody
	// for details.
	CGhost* wghost = new CGhost(*ghost);
	integrator->two.push_back(wghost);
	nadded++;
}
CBody* CSim::at(int ii) { return integrator->read[ii]; }
CBody CSim::copy(int ii) { return *integrator->read[ii]; }
double CSim::H() { return integrator->h; }
int CSim::count() { return nadded; }
void CSim::sort() {
	std::array<std::vector<CBody*>, 2> planets;
	std::array<std::vector<CBody*>, 2> ghosts;
	std::array<std::vector<CBody*>, 2> defs;
	for (int ii = 0; ii < nadded; ii++) {
		if (integrator->one[ii]->Type() == bodyType::planet) {
			planets[0].push_back(integrator->one[ii]);
			planets[1].push_back(integrator->two[ii]);
		}
		else if (integrator->one[ii]->Type() == bodyType::ghost) {
			ghosts[0].push_back(integrator->one[ii]);
			ghosts[1].push_back(integrator->two[ii]);
		}
		else {
			defs[0].push_back(integrator->one[ii]);
			defs[1].push_back(integrator->two[ii]);
		}
	}
	nreal = planets[0].size();
	nghosts = ghosts[0].size();
	ndefs = defs[0].size();
	std::array<std::valarray<CBody*>, 2> bodies;
	bodies[0].resize(nreal + nghosts + ndefs);
	bodies[1].resize(nreal + nghosts + ndefs);
	for (int ii = 0; ii < nreal; ii ++) {
		bodies[0][ii] = planets[0][ii];
		bodies[1][ii] = planets[1][ii];
	}
	for (int ii = nreal; ii < nghosts + nreal; ii ++) {
		bodies[0][ii] = ghosts[0][ii];
		bodies[1][ii] = ghosts[1][ii];
	}
	for (int ii = nreal + nghosts; ii < nghosts + nreal + ndefs; ii ++) {
		bodies[0][ii] = defs[0][ii];
		bodies[1][ii] = defs[1][ii];
	}
	for (int ii = 0; ii < bodies[0].size(); ii ++) {
		bodies[0][ii]->idx = ii;
		bodies[1][ii]->idx = ii;
		integrator->one[ii] = bodies[0][ii];
		integrator->two[ii] = bodies[1][ii];
	}
	integrator->read = integrator->one;
	integrator->write = integrator->two;
}
int CSim::NVirtual() {
	int size = integrator->read.size();
	int ret = 0;
	for (int ii = 0; ii < size; ii ++) {
		if (integrator->read[ii]->Type() == bodyType::def) {
			ret++;
		}
	}
	return ret;
}
int CSim::NReal() {
	const int size = integrator->read.size();
	int ret = 0;
	for (int ii = 0; ii < size; ii ++) {
		if (integrator->read[ii]->Type() != bodyType::def) {
			ret++;
		}
	}
	return ret;
}

void CSim::ofile(const std::string& filename)     { binaryofile = filename; }
void CSim::outputInterval(const double& interval) { write_interval = interval; }
void CSim::binarywrite() {
	binaryout.open(binaryofile, std::ios::binary | std::ios::app);
	for (auto l : output) {
		binaryout.write((char*)&nbodies, 8);
		for (int ii = 0; ii < nbodies; ii ++) {
			for (int jj = 0; jj < 3; jj ++) {
				binaryout.write((char*)&(l[ii].first[jj]), 8);
			}
			for (int jj = 0; jj < 3; jj ++) {
				binaryout.write((char*)&(l[ii].second[jj]), 8);
			}
		}
	}
	binaryout.close();
	write_fix = simTime;
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
		current = integrator->read[ii];
		if (current != NULL && current->Type() != bodyType::def) {
			out << current->writeFormat();
			std::cout << current->writeFormat();
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
			body->Name(name);
			
			print(body->info(), 2);
		}
	}
	debug = gdebug;
	println(green+"done"+res);
	return sim;
}

void CSim::sim() {
	auto start = std::chrono::high_resolution_clock::now();
	binaryout = std::fstream(binaryofile, std::ios::out | std::ios::binary);

	integrator = new Integrator();
	integrator->set(nadded, nghosts);
	bulirschStoer = new BulirschStoer();
	miller = new Miller();

	if (do_main) {
		if (type == simType::basic) {
			calcs.push_back(std::bind(&Integrator::main, this->integrator, std::placeholders::_1, std::placeholders::_2));
		}
		else if (type == simType::miller) {
			calcs.push_back(std::bind(&Integrator::main, this->miller, std::placeholders::_1, std::placeholders::_2));
		}
		else if (type == simType::bulirschStoer) {
			calcs.push_back(std::bind(&Integrator::main, this->bulirschStoer, std::placeholders::_1, std::placeholders::_2));
		}
	}
	ncalcs = calcs.size();
	sort();
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
	sort();
	nbodies = nadded;
	positions.resize(nadded);
	for (int jj = 0; jj < nbodies; jj ++) {
		positions[jj][0].push_back(integrator->write[jj]->r[0]);
		positions[jj][1].push_back(integrator->write[jj]->r[1]);
	}

	int block = nbodies / nthreads;
//			cpu_set_t set;
//			CPU_ZERO(&set);
	std::thread threads[nthreads];
	for (int ii = 0; ii < nthreads - 1; ii ++) {
		threads[ii] = std::thread(&CSim::integrate, this, block * ii, block * (ii + 1));
//				CPU_SET(ii, &set);
//				pthread_setaffinity_np(threads[ii].native_handle(), sizeof(cpu_set_t), &set);
	}
	threads[nthreads - 1] = std::thread(&CSim::integrate, this, block * (nthreads - 1), nbodies);
//			CPU_SET(nthreads - 1, &set);
//			pthread_setaffinity_np(threads[nthreads - 1].native_handle(), sizeof(cpu_set_t), &set);
	integrator->nbodies = nbodies;
#ifdef profiling
	start = std::chrono::high_resolution_clock::now();
#endif
	if (int(integrator->h) > 0) {
		vec a;
		vec v;
		unsigned long long percent;
		int past = 0;
		if (maxTime > 100 * integrator->h) {
			percent = (maxTime - integrator->h) / 100;
		}
		else {
			percent = integrator->h;
		}
		while (simTime < maxTime) {
			if (simTime - write_fix >= write_interval) {
				std::valarray<std::pair<Pos, Vel>> line(nbodies);
				for (int jj = 0; jj < nbodies; jj ++) {
					line[jj] = std::make_pair(integrator->write[jj]->r, integrator->write[jj]->v);
					positions[jj][0].push_back(integrator->write[jj]->r[0]);
					positions[jj][1].push_back(integrator->write[jj]->r[1]);
				}
				output.push_back(line);
				write_fix = simTime;
			}
			tocalc = nthreads;
#if SIMTIME_TYPE == 1
            if (simTime / percent != past && simTime < maxTime) {
                if (!debug) std::cout << '\r' << " Progress: " << std::setw(3) << (simTime * 100) / ((unsigned long long)maxTime - (unsigned long long)(integrator->h)) << std::flush;
                past = simTime / percent;
            }
            if (simTime >= maxTime - h) {
                if (!debug) std::cout << "\r Progress: 100\n";
            }
            simTime += h;
#elif SIMTIME_TYPE == 2
            if ((unsigned long long)simTime / percent != past) {
                if (!debug) std::cout << '\r' << " Progress: " << std::setw(3) << ((unsigned long long)simTime * 100) / ((unsigned long long)maxTime - (unsigned long long)(integrator->h)) << std::flush;
                past = (unsigned long long)simTime / percent;
            }
            if ((unsigned long long)maxTime - (unsigned long long)simTime <= integrator->h) {
                if (!debug) std::cout << "\r Progress: 100\n";
            }
            fetch_add(&simTime, integrator->h);
#endif
			println(in("CSim","sim")+"         Sim time - "+std::to_string(simTime),1);
			while (tocalc > 0) {}					
			if (integrator->read == integrator->one) {
				integrator->read = integrator->two;
				integrator->write = integrator->one;
			}
			else {
				integrator->read = integrator->one;
				integrator->write = integrator->two;
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
	binarywrite();
	for (auto p : positions) {
		PyObject* x = PyList_New(0);
		PyObject* y = PyList_New(0);
		for (auto i : p[0]) {
			PyList_Append(x, Py_BuildValue("d", i));
		}
		for (auto j : p[1]) {
			PyList_Append(y, Py_BuildValue("d", j));
		}
		PyObject_CallFunction(pltPlot, "(OO)", x, y);
		PyObject_CallFunction(pltScatter, "([d],[d])", p[0][0], p[1][0]);
	}
	PyObject_CallMethod(PyObject_CallFunction(PyObject_GetAttrString(plt, (char*)("gca")), "()"), "set_aspect", "s", (char*)("equal"));
	PyObject_CallFunction(pltSavefig, "(s)", (char*)("out.png"));
	Py_Finalize();
}

void CSim::integrate(int min, int max) {
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
			for (int ii = min; ii < max; ii ++) {
				for (int jj = 0; jj < ncalcs; jj ++) {
					calcs[jj](integrator->read[ii], integrator->write[ii]);
				}
				print(in("CSim", "integrate")+"       Old Velocity of {"+cyan+integrator->read[ii]->Name()+res+"} is "+integrator->read[ii]->v.info(3)+"\n", 1);
				print(in("CSim", "integrate")+"       New Velocity of {"+cyan+integrator->write[ii]->Name()+res+"} is "+integrator->write[ii]->v.info(3)+"\n", 1);
				print(in("CSim", "integrate")+"       Old posiiton of {"+cyan+integrator->read[ii]->Name()+res+"} is "+integrator->read[ii]->r.info(3)+"\n", 1);
				print(in("CSim", "integrate")+"       New posiiton of {"+cyan+integrator->write[ii]->Name()+res+"} is "+integrator->write[ii]->r.info(3)+"\n", 1);
			}
			tocalc--;
#ifdef profiling
			cputime += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
#endif
		}
	}
	joinable++;
}

void CSim::init() {
	print("Initializing new "+cyan+bright+"CSim"+res+"...");

	Py_Initialize();
	plt = PyImport_ImportModule("matplotlib.pyplot");
	pltScatter = PyObject_GetAttrString(plt, (char*)("scatter"));
	pltPlot = PyObject_GetAttrString(plt, (char*)("plot"));
	pltSavefig = PyObject_GetAttrString(plt, (char*)("savefig"));
	pltShow = PyObject_GetAttrString(plt, (char*)("show"));
	
	type = simType::basic;
	do_main = true;
	ncalcs = 0;
	
	print(green+" done\n"+res);
}

