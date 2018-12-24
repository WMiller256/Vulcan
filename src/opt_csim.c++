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
#if SIMTIME_TYPE == 1
std::atomic<unsigned long long> simTime;
#elif SIMTIME_TYPE == 2
std::atomic<double> simTime;
#endif
std::atomic<int> tocalc;
std::atomic<int> joinable;

CSim::CSim() {
	init();	
	bRead = bOne;
	bWrite = bTwo;
}
CSim::CSim(int n) {
	init();
	nbodies = n;
	bRead = bOne;
	bWrite = bTwo;
}
CSim::CSim(int n, double max, double step) {
	init();
	nadded = 0;
	nbodies = n;
	h = step;
	tMax = max;
	maxTime = max;
	bRead = bOne;
	bWrite = bTwo;
}
CSim::~CSim() {
}

void CSim::setDebug(int Debug) {
	debug = Debug;
}

void CSim::addBody(CBody* body) {
	bOne.push_back(body);
	bTwo.push_back(body);
	nadded++;
}
void CSim::addGhost(CGhost* ghost) {
//	gOne.push_back(ghost);
//	gTwo.push_back(ghost);
	nadded++;
}
CBody* CSim::at(int ii) {
	return bRead[ii];
}
CBody CSim::copy(int ii) {
	return *bRead[ii];
}

void CSim::fixedHForce(CBody* body, CBody* wbody) {
//	println(in("CSim", "fixedHForce")+"    Calculating net force", 4);//	println(in("CSim", "fixedHForce")+"    Calculating net force", 4);
	Force net(0,0,0);
	vec v;
	vec a;
	double fmagnitude;
	for (int ii = 0; ii < nadded; ii ++) {
		if (bRead[ii] != NULL) {
			if (bRead[ii] != body) {
//				printrln("\n"+in("CSim", "fixedHForce")+"    Target: ", body -> Name(), 5); //				printrln("\n"+in("CSim", "fixedHForce")+"    Target: ", body -> Name(), 5); 
	
				fmagnitude = (G * body -> Mass() * bRead[ii] -> Mass()) / pow(bRead[ii] -> distance(body -> pos), 2);
				net += body -> pos.direction(bRead[ii] -> pos) * fmagnitude; 

//				printrln(in("CSim", "fixedHForce")+"    Magnitude of force between "+body -> Name()+" and "+//					bRead[ii] -> Name()+" is ", scientific(fmagnitude), 4);//					bRead[ii] -> Name()+" is ", scientific(fmagnitude), 4);
//				printrln(in("CSim", "fixedHForce")+"    Net force vector on "+body -> Name()+" is ", body -> net.info(), 4);//				printrln(in("CSim", "fixedHForce")+"    Net force vector on "+body -> Name()+" is ", body -> net.info(), 4);
			}
		}
	}
	a = net / body -> Mass() * h;
	v = wbody -> accelerate(a); 
	wbody -> Position(body -> pos + v + a * (h * 0.5));
	wbody -> fix = simTime;
	wbody -> ncalcs++;

//	println(in("CSim", "fixedHForce")+green+"    Done"+res+" net force", 5);	//	println(in("CSim", "fixedHForce")+green+"    Done"+res+" net force", 5);	
}

int CSim::writeConfiguration(const std::string& filename, bool overwrite) {
	std::ofstream out;
	if (exists(filename) && overwrite == false) {
		if (!prompt("File "+yellow+filename+res+" exists, overwrite? (y|n) ")) {
//			println("File will not be overwritten, exiting "+cyan+"CSim"+yellow+"::"+bright+white+"bWriteConfiguration"+res);//			println("File will not be overwritten, exiting "+cyan+"CSim"+yellow+"::"+bright+white+"bWriteConfiguration"+res);
			return 0;
		}
		else {
//			println("Overwriting "+yellow+filename+res);//			println("Overwriting "+yellow+filename+res);
		}
	}
//	print("Writing file "+yellow+filename+res+"... ");//	print("Writing file "+yellow+filename+res+"... ");
	out.open(filename);
	out << nadded - 1 << "\n";
	CBody* current;
	print_special("bWritestream", 'c', 'k');		// Exclude
	for (int ii = 0; ii < nadded; ii ++) {
		current = bRead[ii];
		if (current != NULL) {
			out << current -> writeFormat();
			std::cout << current -> writeFormat();
		}
		else {
//			error("Body "+std::to_string(ii)+" was "+bright+red+"NULL"+res+" - skipping.", __LINE__, __FILE__);//			error("Body "+std::to_string(ii)+" was "+bright+red+"NULL"+res+" - skipping.", __LINE__, __FILE__);
		}
	}
//	println(green+"done"+res);//	println(green+"done"+res);
	return 0;
}

CSim* CSim::readConfiguration(const std::string& filename) {
	bool check;
	if (!exists(filename)) {
		std::cout << check << std::endl;
//		error("File "+yellow+filename+res+" does not exist.",__LINE__,__FILE__);//		error("File "+yellow+filename+res+" does not exist.",__LINE__,__FILE__);
		return NULL;
	}
//	print("bReading file "+yellow+filename+res+"... ");//	print("bReading file "+yellow+filename+res+"... ");
//	print("\n", 2);//	print("\n", 2);
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

//	println("Number of bodies: "+bright+red+std::to_string(nbodies)+res, 3);//	println("Number of bodies: "+bright+red+std::to_string(nbodies)+res, 3);
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
//			println(line, 3);//			println(line, 3);
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
//				error(" bRead error", __LINE__, __FILE__);//				error(" bRead error", __LINE__, __FILE__);
				return NULL;
			}
			std::getline(file, line); 
//			println(line, 3);//			println(line, 3);
			n++;
			radius = stof(line.substr(line.find_last_of(" ")+1));
			std::getline(file, line); 
//			println(line, 3);//			println(line, 3);
			n++;
			mass = stof(line.substr(line.find_last_of(" ")+1));

			CBody* body = new CBody(mass, radius, speed, x, y, z);
			body -> Name(name);
			
//			print(body -> info(), 2);//			print(body -> info(), 2);
		}
	}
	debug = gdebug;
//	println(green+"done"+res);//	println(green+"done"+res);
	return sim;
}
double CSim::H() {
	return h;
}
int CSim::count() {
	return nadded;
}
int CSim::NReal() {
	return bRead.size();
}
int CSim::NGhost() {
	return gRead.size();
}

void CSim::init() {
//	print("Initializing new "+cyan+bright+"CSim"+res+"...");//	print("Initializing new "+cyan+bright+"CSim"+res+"...");
	tMax = 0.0;
	tCurr = 0.0;
	h = -1.0;
	nadded = 0;
//	print(green+" done\n"+res);//	print(green+" done\n"+res);
}

void CSim::sim(threadmode t) {
	auto start = std::chrono::high_resolution_clock::now();
	bRead = bOne;
	bWrite = bTwo;
	int nreal = NReal();
	int nghost = NGhost();
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
			nbodies = nadded;
			if (nbodies < nthreads) {
				nthreads = nbodies;
			}

			int block = nbodies / nthreads;
			std::thread threads[nthreads];
			for (int ii = 0; ii < nthreads - 1; ii ++) {
				threads[ii] = std::thread(&CSim::threadedFixedH, this, block * ii, block * (ii + 1));
			}
			threads[nthreads - 1] = std::thread(&CSim::threadedFixedH, this, block * (nthreads - 1), nbodies);
#ifdef profiling
			start = std::chrono::high_resolution_clock::now();
#endif
			if (h > 0) {
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
#if SIMTIME_TYPE == 1
					if (simTime / percent != past) {
						std::cout << '\r' << " Progress: " << std::setw(3) << (simTime * 100) / ((unsigned long long)maxTime - (unsigned long long)h) << std::flush;						
						past = simTime / percent;
					}
					if (simTime == maxTime - h) {
						std::cout << "\r Progress: 100\n";
					}
#elif SIMTIME_TYPE == 2
					if ((unsigned long long)simTime / percent != past) {
						std::cout << '\r' << " Progress: " << std::setw(3) << ((unsigned long long)simTime * 100) / ((unsigned long long)maxTime - (unsigned long long)h) << std::flush;						
						past = (unsigned long long)simTime / percent;
					}
					if ((unsigned long long)maxTime - (unsigned long long)simTime <= h) {
						std::cout << "\r Progress: 100\n";
					}
#endif
					tocalc = nthreads;
#if SIMTIME_TYPE == 1
					simTime += h;
#elif SIMTIME_TYPE == 2
					fetch_add(&simTime, h);
#endif
//					println(in("CSim","sim")+"                Sim time - "+std::to_string(simTime),1);//					println(in("CSim","sim")+"                Sim time - "+std::to_string(simTime),1);
					while (tocalc > 0) {}					
					if (bRead == bOne) {
						bRead = bTwo;
						bWrite = bOne;
					}
					else {
						bRead = bOne;
						bWrite = bTwo;
					}
				}
			}
			else {
//				error("{h} is less than or equal to 0.0, cannot simulate.", __LINE__, __FILE__);//				error("{h} is less than or equal to 0.0, cannot simulate.", __LINE__, __FILE__);
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
	vec v;
	vec a;
	double prev = 0;
	int ii = 0;
	CBody* body = NULL;
	CBody* wbody = NULL;
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
				fixedHForce(bRead[ii], bWrite[ii]);
						
//				print(in("CSim", "threadedFixedH")+"       Velocity of {"+cyan+wbody -> Name()+res+"} is "+wbody -> Velocity().info(3)+"\n", 1);//				print(in("CSim", "threadedFixedH")+"       Velocity of {"+cyan+wbody -> Name()+res+"} is "+wbody -> Velocity().info(3)+"\n", 1);
//				print(in("CSim", "threadedFixedH")+"       New posiiton for {"+cyan+wbody -> Name()+res+"} is "+wbody -> pos.info(3)+"\n", 1);//				print(in("CSim", "threadedFixedH")+"       New posiiton for {"+cyan+wbody -> Name()+res+"} is "+wbody -> pos.info(3)+"\n", 1);
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
//	printrln(in("", "unthreadedFixedH(CSim*, double)"), "Simulating", 5);//	printrln(in("", "unthreadedFixedH(CSim*, double)"), "Simulating", 5);
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
				fixedHForce(bRead[ii], bWrite[ii]);
//				printrln(in("", "unthreadedFixedH")+"       Velocity of {"+cyan+body -> Name()+res+"} is ", body -> Velocity().info(3), 1);//				printrln(in("", "unthreadedFixedH")+"       Velocity of {"+cyan+body -> Name()+res+"} is ", body -> Velocity().info(3), 1);
			}
			if (bRead == bOne) {
				bRead = bTwo;
				bWrite = bOne;
			}
			else {
				bRead = bOne;
				bWrite = bTwo;
			}
#ifdef profiling
				cputime += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
#endif
			t += h;
		}
		std::cout << "\r Progress: 100\n";
	}
	else {
//		error("{h} was less than or equal to zero. Cannot simulate",__LINE__,__FILE__);//		error("{h} was less than or equal to zero. Cannot simulate",__LINE__,__FILE__);
		return;
	}
//	printrln(in("", "unthreadedFixedH(CSim*, double)"), green+"Complete"+res, 5);//	printrln(in("", "unthreadedFixedH(CSim*, double)"), green+"Complete"+res, 5);
}
