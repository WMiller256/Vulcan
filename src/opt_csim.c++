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
std::atomic<unsigned long long> simTime;
std::atomic<int> tocalc;
std::atomic<int> next;
std::atomic<int> joinable;

CSim::CSim() {
	init();
	one = new CBody*[nbodies];
	two = new CBody*[nbodies];
	for (int ii = 0; ii < nbodies; ii ++) {
		one[ii] = new CBody();
		two[ii] = new CBody();
	}
	read = one;
	write = two;
}
CSim::CSim(int n) {
	init();
	nbodies = n;
	one = new CBody*[nbodies];
	two = new CBody*[nbodies];
	for (int ii = 0; ii < nbodies; ii ++) {
		one[ii] = new CBody();
		two[ii] = new CBody();
	}
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
	one = new CBody*[nbodies];
	two = new CBody*[nbodies];
	for (int ii = 0; ii < nbodies; ii ++) {
		one[ii] = new CBody();
		two[ii] = new CBody();
	}
	read = one;
	write = two;
}
CSim::~CSim() {
}

void CSim::setDebug(int Debug) {
	debug = Debug;
}

void CSim::addBody(CBody* body) {
	one[nadded] = body;
	two[nadded++] = body;
}
CBody* CSim::at(int ii) {
	return read[ii];
}
CBody CSim::copy(int ii) {
	return *read[ii];
}

void CSim::force(CBody* body) {
//	println(in("CSim", "force")+"    Calculating net force", 4);//	println(in("CSim", "force")+"    Calculating net force", 4);
	body -> net.zero() ;
	double fmagnitude;
	for (int ii = 0; ii < nadded; ii ++) {
		if (read[ii] != NULL) {
			if (read[ii] != body) {
//				printrln("\n"+in("CSim", "force")+"    Target: ", body -> Name(), 5); //				printrln("\n"+in("CSim", "force")+"    Target: ", body -> Name(), 5); 
				fmagnitude = (G * body -> Mass() * read[ii] -> Mass()) / pow(read[ii] -> distance(body -> pos), 2);
				body -> net += body -> pos.direction(read[ii] -> pos) * fmagnitude;

//				printrln(in("CSim", "force")+"    Magnitude of force between "+body -> Name()+" and "+//					read[ii] -> Name()+" is ", scientific(fmagnitude), 4);//					read[ii] -> Name()+" is ", scientific(fmagnitude), 4);
//				printrln(in("CSim", "force")+"    Net force vector on "+body -> Name()+" is ", body -> net.info(), 4);//				printrln(in("CSim", "force")+"    Net force vector on "+body -> Name()+" is ", body -> net.info(), 4);
			}
		}
	}
//	println(in("CSim", "force")+green+"    Done"+res+" net force", 5);	//	println(in("CSim", "force")+green+"    Done"+res+" net force", 5);	
}
Force CSim::force(CBody body) {
//	println(in("CSim", "force")+"    Calculating net force", 5);//	println(in("CSim", "force")+"    Calculating net force", 5);
	Force net(0,0,0);
	double fmagnitude;
	for (int ii = 0; ii < nadded; ii ++) {
		if (read[ii] != NULL) {
			if (*read[ii] != body) {
//				printrln("\n"+in("CSim", "force")+"    Target: ", body.Name(), 5); //				printrln("\n"+in("CSim", "force")+"    Target: ", body.Name(), 5); 
				fmagnitude = (G * body.Mass() * read[ii] -> Mass()) / pow(read[ii] -> distance(body.pos), 2);
				net += body.pos.direction(read[ii] -> pos) * fmagnitude;

//				printrln(in("CSim", "force")+"    Magnitude of force between "+body.Name()+" and "+//					read[ii] -> Name()+" is ", scientific(fmagnitude), 4);//					read[ii] -> Name()+" is ", scientific(fmagnitude), 4);
//				printrln(in("CSim", "force")+"    Net force vector on "+body.Name()+" is ", net.info(), 4);//				printrln(in("CSim", "force")+"    Net force vector on "+body.Name()+" is ", net.info(), 4);
			}
		}
	}
//	println(in("CSim", "force")+green+"    Done"+res+" net force", 5);	//	println(in("CSim", "force")+green+"    Done"+res+" net force", 5);	
	return net;
}

int CSim::writeConfiguration(const std::string& filename, bool overwrite) {
	std::ofstream out;
	if (exists(filename) && overwrite == false) {
		if (!prompt("File "+yellow+filename+res+" exists, overwrite? (y|n) ")) {
//			println("File will not be overwritten, exiting "+cyan+"CSim"+yellow+"::"+bright+white+"writeConfiguration"+res);//			println("File will not be overwritten, exiting "+cyan+"CSim"+yellow+"::"+bright+white+"writeConfiguration"+res);
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
	print_special("Writestream", 'c', 'k');		// Exclude
	for (int ii = 0; ii < nadded; ii ++) {
		current = read[ii];
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
//	print("Reading file "+yellow+filename+res+"... ");//	print("Reading file "+yellow+filename+res+"... ");
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
//				error(" Read error", __LINE__, __FILE__);//				error(" Read error", __LINE__, __FILE__);
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
	switch(t) {
		case single:
#ifdef profiling
			start = std::chrono::high_resolution_clock::now();
#endif
			simulate((unsigned long)tMax);
#ifdef profiling
			simulationtime += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
#endif
			break;
		case manual:
			simTime = 0;
			tocalc = 0;
			joinable = 0;
			next = 0;
			nbodies = nadded;
			if (nadded < nthreads) {
				nthreads = nadded;
			}
			std::thread threads[nthreads];
			for (int ii = 0; ii < nthreads; ii ++) {
				threads[ii] = std::thread(&CSim::man_simulate, this);
			}
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
					if (simTime / percent != past) {
						std::cout << '\r' << " Progress: " << std::setw(3) << (simTime * 100) / ((unsigned long long)maxTime - (unsigned long long)h) << std::flush;						
						past = simTime / percent;
					}
					if (simTime == maxTime - h) {
						std::cout << "\r Progress: 100\n";
					}
				    tocalc = nbodies;
				    next = 0;
					simTime += h;
//					println(in("CSim","sim")+"                Sim time - "+std::to_string(simTime),1);//					println(in("CSim","sim")+"                Sim time - "+std::to_string(simTime),1);
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
//				error("{h} is less than or equal to 0.0, cannot simulate.", __LINE__, __FILE__);//				error("{h} is less than or equal to 0.0, cannot simulate.", __LINE__, __FILE__);
			}
#ifdef profiling
			simulationtime += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
#endif
			while (joinable < nbodies) {
			}
			for (int ii = 0; ii < nthreads; ii ++) {
				threads[ii].join();
			}
			break;
	}
}
void CSim::man_simulate() {
	int tot = 0;
	vec v;
	vec a;
	double prev = 0;
	int ii;
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
			while (tocalc > 0) {
				ii = next++;
				if (ii >= nbodies) {
					break;
				} 
				body = read[ii];
				wbody = write[ii];
				a = force(*body) / body -> Mass() * h;
				v = wbody -> accelerate(a); 
				wbody -> Position(body -> pos + v * h + a * (h * 0.5));
				wbody -> fix = simTime;
//				print(in("CSim", "man_simulate")+"       Velocity of {"+cyan+wbody -> Name()+res+"} is "+wbody -> Velocity().info(3)+"\n", 1);//				print(in("CSim", "man_simulate")+"       Velocity of {"+cyan+wbody -> Name()+res+"} is "+wbody -> Velocity().info(3)+"\n", 1);
//				print(in("CSim", "man_simulate")+"       New posiiton for {"+cyan+wbody -> Name()+res+"} is "+wbody -> pos.info(3)+"\n", 1);//				print(in("CSim", "man_simulate")+"       New posiiton for {"+cyan+wbody -> Name()+res+"} is "+wbody -> pos.info(3)+"\n", 1);
				tocalc--;
				wbody -> ncalcs++;
			}
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
void CSim::simulate(unsigned long end) {
//	printrln(in("", "simulate(CSim*, double)"), "Simulating", 5);//	printrln(in("", "simulate(CSim*, double)"), "Simulating", 5);
	unsigned long t = 0.0;
	double h = H();
	nbodies = count();
	int past = 0;
	CBody* body;
	vec a[nbodies];
	vec v[nbodies];
	CBody** bodies = new CBody*[nbodies];
	for (int ii = 0; ii < nbodies; ii ++) {
		bodies[ii] = at(ii);
	}
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
				body = bodies[ii];
				a[ii] = force(*body) / body -> Mass() * h;
				v[ii] = body -> accelerate(a[ii]); 
//				printrln(in("", "simulate")+"       Velocity of {"+cyan+body -> Name()+res+"} is ", body -> Velocity().info(3), 1);//				printrln(in("", "simulate")+"       Velocity of {"+cyan+body -> Name()+res+"} is ", body -> Velocity().info(3), 1);
			}
			for (int ii = 0; ii < nbodies; ii ++) {
				body = bodies[ii];
				body -> Position(body -> pos + v[ii] * h + a[ii] * (h * 0.5));
//				printrln(in("", "Simulate")+"       New posiiton for {"+cyan+body -> Name()+res+"} is ", body -> pos.info(3), 1);//				printrln(in("", "Simulate")+"       New posiiton for {"+cyan+body -> Name()+res+"} is ", body -> pos.info(3), 1);
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
//	printrln(in("", "simulate(CSim*, double)"), green+"Complete"+res, 5);//	printrln(in("", "simulate(CSim*, double)"), green+"Complete"+res, 5);
}
