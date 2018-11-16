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
				fmagnitude = (G * body -> Mass() * bodies[ii] -> Mass()) / pow(bodies[ii] -> distance(body), 2);
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
				fmagnitude = (G * body.Mass() * bodies[ii] -> Mass()) / pow(bodies[ii] -> distance(body), 2);
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
			simulate(this, tMax);
			break;
		case manual:
			if (nadded < nthreads) {
				nthreads = nadded;
			}
			int data_pipe[2];
			int ping[2];
			pipe(data_pipe);
			pipe(ping);
			double* time = new double(0);
			std::cout << " Using " << nthreads << " threads" << std::endl;
			std::thread threads[nthreads];
			for (int ii = 0; ii < nthreads; ii ++) {
				threads[ii] = std::thread(man_simulate, this, data_pipe, ping, ii, time);
				println("Thread "+std::to_string(ii+1)+" initialized.");
			}
			if (h > 0.0) {
				int wait;
				while (*time < maxTime) {
					wait = 0;
					*time += h;
					while (wait++ < 5){}
				}
			}
			else {
				error("{h} is less than or equal to 0.0, cannot simulate.", __LINE__, __FILE__);
			}
			for (int ii = 0; ii < nthreads; ii ++) {
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
void man_simulate(CSim* sim, int* data_pipe, int* ping, int ii, double* simTime) {
#ifdef using_hash
	error ("Funtion "+in("", "simulate(CSim*, CBody*, double)")+" is invalid when using hash table. Use "+in("", "simulate(Hash*, CBody*, double)")+" instead", __LINE__, __FILE__);
#else
	printrln(in("", "simulate(CSim*, CBody*, double)"), "Simulating", 5);
	CBody* body;
	//rpos* p = new rpos;
	double H = sim -> H();
	double h;
	body = sim -> at(ii);
	std::string name = body -> Name();
	vec v;
	vec a;
	double time = *simTime;
	double prev = time;
	while (time < maxTime) {
#ifdef profiling
		auto start = std::chrono::high_resolution_clock::now();
#endif
		time = *simTime;
		h = (time - prev);
		prev = time;
		a = sim -> force(*body) / body -> Mass();
		//if (ii == 0) {
		//	std::cout << body -> Name()+a.info()+" "+std::to_string(h)+"\n";
		//}		
		v = body -> accelerate(a * h); body -> Position(body -> pos + v * h + a * h * h * 0.5);
#ifdef profiling
		cputime += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
#endif
		printrln(in("", "man_simulate")+"       Velocity of {"+cyan+body -> Name()+res+"} is ", body -> Velocity().info(3), 4);
		printrln(in("", "man_Simulate")+"       New posiiton for {"+cyan+body -> Name()+res+"} is ", body -> pos.info(3), 4);
	}
	printrln(in("", "simulate(CSim*, CBody*, double)"), green+"Complete"+res, 5);
#endif
}
void simulate(CSim* sim, double end) {
#ifdef using_hash
	error ("Funtion "+in("", "simulate(CSim*, double)")+" is invalid when using hash table.", __LINE__, __FILE__);
#else
	printrln(in("", "simulate(CSim*, double)"), "Simulating", 5);
	double t = 0.0;
	double h = sim -> H();
	nbodies = sim -> count();
	CBody* body;
	vec a;
	vec v;
	CBody** bodies = new CBody*[nbodies];
	for (int ii = 0; ii < nbodies; ii ++) {
		bodies[ii] = sim -> at(ii);
	}
	if (h > 0.0) {
		while (t < end) {
			for (int ii = 0; ii < nbodies; ii ++) {
				body = bodies[ii];
#ifdef profiling
				auto start = std::chrono::high_resolution_clock::now();
#endif
				a = sim -> force(*body) / body -> Mass();
				v = body -> accelerate(a * h);
				body -> Position(body -> pos + v * h + a * h * h * 0.5);
#ifdef profiling
				cputime += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
#endif
				printrln(in("", "simulate")+"       Velocity of {"+cyan+body -> Name()+res+"} is ", body -> Velocity().info(3), 4);
				printrln(in("", "Simulate")+"       Previous posiiton for {"+cyan+body -> Name()+res+"} is ", p.info(3), 4);
				printrln(in("", "Simulate")+"       New posiiton for {"+cyan+body -> Name()+res+"} is ", body -> pos.info(3), 4);
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
