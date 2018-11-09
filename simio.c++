#include "simio.h"

void Hash::write(const std::string&	filename) {
	std::ofstream out;
	if (exists(filename)) {
		if (!prompt("File exists, overwrite? (y|n) ")) {
			println("File will not be overwritten, exiting "+cyan+"Hash"+yellow+"::"+bright+white+"write"+res);
			return;
		}
		else {
			println("Overwriting "+yellow+filename+res);
		}
	}
	print("Writing file "+yellow+filename+res+"... ");
	out.open(filename);
	out << size << "\n" << minradius << "\n" << maxradius << "\n";
	nodeptr current = new node;
	for (int ii = 0; ii < size; ii ++) {
		current = table[ii];
		if (current != NULL) {
			int depth = current -> bodies.size();
			for (int kk = 0; kk < depth; kk ++) {
				if (current -> bodies.at(kk) != NULL) {
					out << current -> bodies.at(kk) -> writeFormat();
				}
				else {
					std::cout << red+white_back << " Error " << black_back << " body " << kk;
					std::cout << " in {bodies} at " << current << ". Exiting." << res << std::endl;
					return;
				}
			}
		}
	}
	println(green+"done"+res);
}

Hash* read(const std::string& filename) {
	if (!exists(filename)) {
		std::cout << red+white_back << " Error " << black_back << " file does not exist." << std::endl;
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
	Hash* hash = new Hash(nbodies);

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
			hash -> addNode(body);
			print(body -> info(), 2);
		}
	}
	debug = gdebug;
	println(green+"done"+res);
	return hash;
}

std::string CBody::writeFormat(format f) {
	std::string formatted;
	switch(f) {
		case text:
			formatted.append("Body\n");
			formatted.append("\t Position ("+std::to_string(x)+", "+std::to_string(y)+", "+std::to_string(z)+")\n");
			formatted.append("\t Radius   "+std::to_string(radius)+"\n");
			formatted.append("\t Mass     "+std::to_string(mass)+"\n");
			formatted.append("\t Speed    "+std::to_string(speed)+"\n\n");
			break;
	}
	print("--------- "+cyan+"Writestream "+res+"--------- \n"+formatted+"------------------------------- \n", 2);
	return formatted;
}

bool exists(const std::string& filename) {
	return std::experimental::filesystem::exists(filename);
}

bool prompt(const std::string& p) {
	char in;
	std::cout << std::flush;
	std::cout << p << std::flush;
	std::cin >> in;
	if (in == 'y') {
		return true;
	}
	else {
		return false;
	}
}

void print(const std::string& s, int depth) {
	if (debug >= depth && depth != -1) {
		std::cout << s << std::flush;
	}
}
void print(const int& i, int depth) {
	if (debug >= depth && depth != -1) {
		std::cout << i << std::flush;
	}
}
void print(const double& d, int depth) {
	if (debug >= depth && depth != -1) {
		std::cout << d << std::flush;
	}
}
void print(const float& f, int depth) {
	if (debug >= depth && depth != -1) {
		std::cout << f << std::flush;
	}
}
void println(const std::string& s, int depth) {
	if (debug >= depth && depth != -1) {
		print(s, depth);
		std::cout << std::endl;
	}
}
void println(const double& d, int depth) {
	if (debug >= depth && depth != -1) {
		print(d, depth);
		std::cout << std::endl;
	}
}
void println(const float& f, int depth) {
	if (debug >= depth && depth != -1) {
		print(f, depth);
		std::cout << std::endl;
	}
}
void println(const int& i, int depth) {
	if (debug >= depth && depth != -1) {
		print(i, depth);
		std::cout << std::endl;
	}
}

void printr(const std::string& l, const std::string r, int depth) {
	if (debug >= depth && depth != -1) {
		int left = nchar(l);
		int right = nchar(r);
		int termwidth = winwidth();
		if (termwidth < left + right) {
			print(l+r, depth);
		}
		else {
			int middle = termwidth - (left + right)-1;
			std::cout << l;
			for (int ii = 0; ii < middle; ii ++) {
				std::cout << " ";
			}
			std::cout << r;
		}		
	}	
}
void printrln(const std::string& l, const std::string r, int depth) {
	if (debug >= depth && depth != -1) {
		printr(l, r, depth);
		if (nchar(l) + nchar(r) < winwidth()) {
			std::cout << std::endl;
		}
	}
}

int nchar(const std::string& s) { // Finds the number of alphanumeric characters in the given string
	int ret = 0;
	std::string str = stripcolors(s);
	int length = str.length();
	for (int ii = 0; ii < length; ii ++) {
		if (str[ii] >= ' ' && str[ii] <= '~') {
			ret++;
		}
	}
	return ret;
}

int winheight() {
    int width;
    int height;
    initscr();
    getmaxyx(stdscr, height, width);
    endwin();
    return height;
}
int winwidth() {
    int width;
    int height;
    initscr();
    getmaxyx(stdscr, height, width);
    endwin();
    return width;
}
void print_special(const std::string& str, const char& fore, const char& back) {
    int termwidth = winwidth();
    std::cout << bright;
    if (back == 'g') {
        std::cout << green_back;
    }
    else if (back == 'c') {
        std::cout << cyan_back;
    }
    else if (back == 'w') {
        std::cout << white_back;
    }
    else if (back == 'b') {
        std::cout << blue_back;
    }
    else if (back == 'y') {
        std::cout << yellow_back;
    }
    else if (back == 'm') {
        std::cout << magenta_back;
    }
    else if (back == 'r') {
        std::cout << red_back;
    }
    else if (back == 'k') {
        std::cout << black_back;
    }
    if (fore == 'g') {
        std::cout << green;
    }
    else if (fore == 'c') {
        std::cout << cyan;
    }
    else if (fore == 'w') {
        std::cout << white;
    }
    else if (fore == 'b') {
        std::cout << blue;
    }
    else if (fore == 'y') {
        std::cout << yellow;
    }
    else if (fore == 'm') {
        std::cout << magenta;
    }
    else if (fore == 'r' || fore == 'e') {
        std::cout << red;
    }
    else if (fore == 'k') {
        std::cout << black;
    }
    if (str.length() < termwidth) {
        for (int ii = 0; ii < (termwidth-str.length())/2; ii++) {
            std::cout << " ";
        }
    }
    std::cout << str;
    if (str.length() < termwidth) {
        for (int ii = 0; ii < (termwidth-str.length())/2; ii++) {
            std::cout << " ";
        }
    }
    std::cout << res << std::endl;
}
