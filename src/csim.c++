#include "csim.h"
#include "simio.h"

bool warnings = true;
int nthreads = 8;
int debug = 0;
int nbodies = 9;
double minradius = std::numeric_limits<double>::infinity();
double maxradius = 0;
double blockwidth;
long long cputime = 0;
long long mtxtime = 0;
long long waittime = 0;
long long nCOMcalls = 0;
double cofactor = 10000;
double maxTime = 0;
std::atomic<unsigned long long> simTime;
std::atomic<int> tocalc;
std::atomic<int> toput;
std::atomic<int> joinable;

CSim::CSim() {
	init();
#ifdef using_hash
	bodies = new Hash(nbodies);
#else 
	bodies = new CBody*[nbodies];
	for (int ii = 0; ii < nbodies; ii ++) {
		bodies[ii] = new CBody();
	}
#endif
}
CSim::CSim(int n) {
	init();
	nbodies = n;
#ifdef using_hash
	bodies = new Hash(n);
#else 
	bodies = new CBody*[nbodies];
	for (int ii = 0; ii < nbodies; ii ++) {
		bodies[ii] = new CBody();
	}
#endif
}
CSim::CSim(int n, double max, double step) {
	init();
	nadded = 0;
	nbodies = n;
	h = step;
	tMax = max;
	maxTime = max;
#ifdef using_hash
	bodies = new Hash(n, h);
#else 
	bodies = new CBody*[nbodies];
	for (int ii = 0; ii < nbodies; ii ++) {
		bodies[ii] = new CBody();
	}
#endif
}
CSim::~CSim() {
}

void CSim::setDebug(int Debug) {
	debug = Debug;
}

void CSim::addBody(CBody* body) {
#ifdef using_hash
	bodies -> addNode(body);
#else
	bodies[nadded++] = body;
#endif
}
CBody* CSim::at(int ii) {
#ifndef using_hash
	return bodies[ii];
#else
	error("Function "+in("CSim","at")+" is invalid when using (Hash) table.", __LINE__, __FILE__);
	return NULL;
#endif
}
CBody CSim::copy(int ii) {
#ifndef using_hash
	return *bodies[ii];
#else
	error("Function "+in("CSim","copy")+" is invalid when using (Hash) table.", __LINE__, __FILE__);
	return NULL;
#endif
}
void CSim::step() {
#ifdef using_hash
	bodies -> step();
#else
	CBody* body;
	for (int ii = 0; ii < nadded; ii ++) {
		body = bodies[ii];
		force(body);
		body -> Position(body -> Velocity(body -> net / body -> Mass() * h) * h);
	}
#endif
}
void CSim::force(CBody* body) {
	println(in("CSim", "force")+"    Calculating net force", 5);
	body -> net.zero() ;
	double fmagnitude;
	for (int ii = 0; ii < nadded; ii ++) {
		if (bodies[ii] != NULL) {
			if (bodies[ii] != body) {
				printrln("\n"+in("CSim", "force")+"    Target: ", body -> Name(), 5); 
				fmagnitude = (G * body -> Mass() * bodies[ii] -> Mass()) / pow(bodies[ii] -> distance(body -> pos), 2);
				body -> net += body -> pos.direction(bodies[ii] -> pos) * fmagnitude;

				printrln(in("CSim", "force")+"    Magnitude of force between "+body -> Name()+" and "+
					bodies[ii] -> Name()+" is ", scientific(fmagnitude), 4);
				printrln(in("CSim", "force")+"    Net force vector on "+body -> Name()+" is ", body -> net.info(), 4);
			}
		}
	}
	println(in("CSim", "force")+green+"    Done"+res+" net force", 5);	
}
Force CSim::force(CBody body) {
	println(in("CSim", "force")+"    Calculating net force", 5);
	Force net(0,0,0);
	double fmagnitude;
	for (int ii = 0; ii < nadded; ii ++) {
		if (bodies[ii] != NULL) {
			if (*bodies[ii] != body) {
				printrln("\n"+in("CSim", "force")+"    Target: ", body.Name(), 5); 
				fmagnitude = (G * body.Mass() * bodies[ii] -> Mass()) / pow(bodies[ii] -> distance(body.pos), 2);
				net += body.pos.direction(bodies[ii] -> pos) * fmagnitude;

				printrln(in("CSim", "force")+"    Magnitude of force between "+body.Name()+" and "+
					bodies[ii] -> Name()+" is ", scientific(fmagnitude), 4);
				printrln(in("CSim", "force")+"    Net force vector on "+body.Name()+" is ", net.info(), 4);
			}
		}
	}
	println(in("CSim", "force")+green+"    Done"+res+" net force", 5);	
	return net;
}

Force CSim::mforce(CBody body) {
	print(in("CSim", "mforce")+"    Calculating net force\n", 5);
	Force net(0,0,0);
	double fmagnitude;
	for (int ii = 0; ii < nadded; ii ++) {
		if (bodies[ii] != NULL) {
			if (*bodies[ii] != body) {
				printrln("\n"+in("CSim", "force")+"    Target: ", body.Name(), 5); 
				fmagnitude = (G * body.Mass() * bodies[ii] -> Mass()) / pow(bodies[ii] -> distance(body.pos), 2);
				net += body.pos.direction(bodies[ii] -> pos) * fmagnitude;

				printrln(in("CSim", "force")+"    Magnitude of force between "+body.Name()+" and "+
					bodies[ii] -> Name()+" is ", scientific(fmagnitude), 4);
				printrln(in("CSim", "force")+"    Net force vector on "+body.Name()+" is ", net.info(), 4);
			}
		}
	}
	println(in("CSim", "force")+green+"    Done"+res+" net force", 5);	
	return net;
}

void CSim::printForces() {
#ifdef using_hash
	bodies -> printForces();
#endif
}
void CSim::writeConfiguration(const std::string& filename, bool overwrite) {
#ifdef using_hash
	bodies -> write(filename);
#else
	std::ofstream out;
	if (exists(filename) && overwrite == false) {
		if (!prompt("File exists, overwrite? (y|n) ")) {
			println("File will not be overwritten, exiting "+cyan+"CSim"+yellow+"::"+bright+white+"writeConfiguration"+res);
			return;
		}
		else {
			println("Overwriting "+yellow+filename+res);
		}
	}
	print("Writing file "+yellow+filename+res+"... ");
	out.open(filename);
	out << nadded - 1 << "\n" << minradius << "\n" << maxradius << "\n";
	CBody* current;
	for (int ii = 0; ii < nadded; ii ++) {
		current = bodies[ii];
		if (current != NULL) {
			out << current -> writeFormat();
		}
		else {
			error("Body "+std::to_string(ii)+" was "+bright+red+"NULL"+res+" - skipping.", __LINE__, __FILE__);
		}
	}
	println(green+"done"+res);
#endif
}

CSim* CSim::readConfiguration(const std::string& filename) {
#ifdef using_hash
	bodies = readfile(filename);
	return NULL;
#else
	if (!exists(filename)) {
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

	std::getline(file, line);
	n++;
	minradius = stof(line);

	std::getline(file, line);
	n++;
	maxradius = stof(line);
	
	blockwidth = (maxradius - minradius) / (nbodies - 1);
	println("  {minradius}:  "+magenta+bright+std::to_string(minradius)+res+"\n  {maxradius}:  "+
		bright+magenta+std::to_string(maxradius)+res+"\n  {blockwidth}: "+bright+magenta+
		std::to_string(blockwidth)+res, 3);
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
			std::size_t start = line.find("(");
			if (start != std::string::npos) {
				x = stof(line.substr(start+1,line.find(",") - start-1));
				y = stof(line.substr(line.find(",")+2, line.find_last_of(",")-1));
				z = stof(line.substr(line.find_last_of(",")+2,line.find(")")-1));
			}
			else {
				println(red+white_back+" Error "+res+" File error at line "+std::to_string(n)+" in file "+yellow+filename+res);
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
			std::getline(file, line); 
			println(line, 3);
			n++;
			speed = stof(line.substr(line.find_last_of(" ")+1));
			CBody* body = new CBody(mass, radius, speed, x, y, z);
			body -> Name(name);
			
			print(body -> info(), 2);
		}
	}
	debug = gdebug;
	println(green+"done"+res);
	return sim;
#endif
}
double CSim::H() {
	return h;
}
int CSim::count() {
	return nadded;
}

void CSim::init() {
	print("Initializing new "+cyan+bright+"CSim"+res+"...");
	tMax = 0.0;
	tCurr = 0.0;
	h = -1.0;
	print(green+" done\n"+res);
}

void sim(Hash* bodies, double tMax, threadmode t) {
#ifdef using_hash
	switch(t) {
		case single:
			bodies -> sim(tMax);
			break;
		case manual:
			if (nbodies < nthreads) {
				nthreads = nbodies;
			}
			std::thread threads[nthreads];
			for (int ii = 0; ii < nthreads; ii ++) {
				CBody* body = bodies -> l[ii];
				threads[ii] = std::thread(simulate, bodies, body, tMax);
			}
			for (int ii = 0; ii < nthreads; ii ++) {
				threads[ii].join();
			}
			break;
	}
#else
	error("Function "+in("","sim(Hash, double, threadmode)")+" is invalid when not using hash table.", __LINE__, __FILE__);
#endif
}
void CSim::sim(threadmode t) {
	std::cout << " Starting simulation " << std::endl;
#ifdef using_hash
	error("Function "+in("","sim(CSim*, double, threadmode)")+" is invalid when using hash table. Use "+in("","sim(Hash*, double, threadmode)")+" instead", __LINE__, __FILE__);
#else
	switch(t) {
		case single:
			simulate((unsigned long)tMax);
			break;
		case manual:
			simTime = 0;
			tocalc = 0;
			toput = 0;
			joinable = 0;
			nbodies = nadded - 1;
			if (nadded < nthreads) {
				nthreads = nadded;
			}
			std::cout << " Using " << nthreads << " threads" << std::endl;
			std::thread threads[nthreads];
			for (int ii = 1; ii < nthreads; ii ++) {
				threads[ii] = std::thread(&CSim::man_simulate, this, ii);
				println("Thread "+std::to_string(ii)+" initialized.");
			}
			if (int(h) > 0) {
				int tot = 0;
				CBody* body = at(0);
				vec a;
				vec v;
				double mass = body -> Mass();
				unsigned long percent;
				if (maxTime > 100) {
					percent = (maxTime - 1) / 100;
				}
				else {
					percent = 1;
				}
				while (simTime < maxTime) {
					if (simTime % percent == 0) {
						std::cout << '\r' << " Progress: " << std::setw(3) << (simTime * 100) / ((unsigned long)maxTime - 1) << std::flush;
					}
					if (simTime == maxTime - 1) {
						std::cout << "\r Progress: 100\n";
					}
				    tocalc = nbodies;
					toput = nbodies;
					simTime += h;
					tot++;
					println(in("CSim","sim")+"          Sim time - "+std::to_string(simTime),1);
					a = force(*body) / mass * h;
					v = body -> accelerate(a); 
					print(in("CSim","sim")+"          {a} - "+a.info(3)+
						"\n                      {v} - "+v.info(3)+"\n                      {pos} - "+body -> pos.info(3)+"\n", 1);
					while (tocalc > 0) {
					}
					body -> Position(body -> pos + v * h + a * (h * 0.5));
					while (toput > 0) {
					}
				}
				std::cout << " "+body->Name()+" {tot} - "+std::to_string(tot)+"\n";
			}
			else {
				error("{h} is less than or equal to 0.0, cannot simulate.", __LINE__, __FILE__);
			}
			while (joinable < nbodies) {
			}
			for (int ii = 1; ii < nthreads; ii ++) {
				threads[ii].join();
				std::cout << "Thread " << ii << " finished" << std::endl;
			}
			break;
	}
#endif
}
void simulate(Hash* bodies, CBody* body, double t) {
#ifdef using_hash
	h -> sim(body, t);
#else
	error ("Funtion "+in("", "simulate(Hash*, CBody*, double)")+" is invalid when not using hash table. Use "+in("", "simulate(CSim*, CBody*, double)")+" instead", __LINE__, __FILE__);
#endif
}
void CSim::man_simulate(int ii) {
#ifdef using_hash
	error ("Funtion "+in("", "simulate(CSim*, CBody*, double)")+" is invalid when using hash table. Use "+in("", "simulate(Hash*, CBody*, double)")+" instead", __LINE__, __FILE__);
#else
	printrln(in("", "simulate(CSim*, CBody*, double)"), "Simulating", 1);
	CBody* body;
	//rpos* p = new rpos;
	body = at(ii);
	std::string name = body -> Name();
	long h = body -> h;
	int tot = 0;
	int decs = 0;
	vec v;
	vec a;
	double mass = body -> Mass();
	double prev = 0;
	bool calc = false;
	while (simTime == 0) {}				// Wait for the simulation to start
	while (true) {
#ifdef profiling
		auto start = std::chrono::high_resolution_clock::now();
#endif
		if (simTime - prev > 0) {
			prev = simTime;
			if (calc = (simTime - body -> fix >= h)) {
				tot++;
				a = force(*body) / mass * h;
				v = body -> accelerate(a); 
			}
			tocalc--;
			decs++;
			while (tocalc > 0) {
				if (simTime == maxTime) {
					break;
				}
			}
			if (calc) {
				body -> Position(body -> pos + v * h + a * (h * 0.5));
				body -> fix = simTime;
			}
			toput--;
			if (simTime == maxTime) {
				break;
			}
		}
#ifdef profiling
		cputime += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
#endif
		print(in("", "man_simulate")+"       Velocity of {"+cyan+body -> Name()+res+"} is "+body -> Velocity().info(3)+"\n", 1);
		print(in("", "man_Simulate")+"       New posiiton for {"+cyan+body -> Name()+res+"} is "+body -> pos.info(3)+"\n", 1);
	}
	if (body -> fix != simTime) {
		std::cout << "   "+std::to_string(ii)+" "+name+" had a problem finishing :(\n";
	}
	joinable++;
	std::cout << "  "+body -> Name()+" {tot} - "+std::to_string(tot)+"\n";
	printrln(in("", "simulate(CSim*, CBody*, double)"), green+"Complete"+res, 1);
#endif
}
void CSim::simulate(unsigned long end) {
#ifdef using_hash
	error ("Funtion "+in("", "simulate(CSim*, double)")+" is invalid when using hash table.", __LINE__, __FILE__);
#else
	printrln(in("", "simulate(CSim*, double)"), "Simulating", 5);
	unsigned long t = 0.0;
	double h = H();
	nbodies = count();
	CBody* body;
	vec a[nbodies];
	vec v[nbodies];
	CBody** bodies = new CBody*[nbodies];
	for (int ii = 0; ii < nbodies; ii ++) {
		bodies[ii] = at(ii);
	}
	if (h > 0.0) {
		unsigned long percent = (end - 1) / 100;
		while (t < end) {
			if ((unsigned long)t % percent == 0) {
				std::cout << '\r' << " Progress: " << std::setw(3) << (t * 100) / (end - 1) << std::flush;
				if (t / end == 1) {
					std::cout << "\r Progress: 100\n";
				} 
			}
			for (int ii = 0; ii < nbodies; ii ++) {
				body = bodies[ii];
#ifdef profiling
				auto start = std::chrono::high_resolution_clock::now();
#endif
				a[ii] = force(*body) / body -> Mass() * h;
				v[ii] = body -> accelerate(a[ii]); 
#ifdef profiling
				cputime += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
#endif
				printrln(in("", "simulate")+"       Velocity of {"+cyan+body -> Name()+res+"} is ", body -> Velocity().info(3), 4);
				printrln(in("", "Simulate")+"       New posiiton for {"+cyan+body -> Name()+res+"} is ", body -> pos.info(3), 4);
			}
			for (int ii = 0; ii < nbodies; ii ++) {
				body = bodies[ii];
				body -> Position(body -> pos + v[ii] * h + a[ii] * (h * 0.5));
			}
			t += h;
		}
	}
	else {
		error("{h} was less than or equal to zero. Cannot simulate",__LINE__,__FILE__);
		return;
	}
	printrln(in("", "simulate(CSim*, double)"), green+"Complete"+res, 5);
#endif
}
