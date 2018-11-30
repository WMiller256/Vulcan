#include "simio.h"


std::string in(std::string prefix, std::string function) {
	std::string ret;
	if (prefix != "") {
		ret = "  "+cyan+prefix+yellow+"::"+bright+white+function+res+" ";
	}
	else {
		ret = "  "+bright+white+function+res+" ";
	}
	return ret;
}

void error(const std::string& s, const int line, const char* file) {
	printf(std::string(red+white_back+" Error "+res+" [File "+yellow+"%s"+res+" line "+bright+red+"%d"+
		res+"] - "+s+"\n").c_str(), file, line);
}
void warning(const std::string& s, const int line, const char* file) {
	printf(std::string(bright+blue+white_back+" Warning "+res+" [File "+yellow+"%s"+res+" line "+bright+red+"%d"
		+res+"] - "+s+"\n").c_str(), file, line);
}

std::string scientific(double d, int precision) {
	std::stringstream ret;
	ret << std::scientific << std::setprecision(precision) << d;
	return ret.str();
}

std::string stripcolors(const std::string& str) {
	std::string s = str;
	int found = s.find('\033');
	while (found != std::string::npos) {
		if (iscolor(s.substr(found + 1, 3))) {
			s.erase(found, 4);
		}
		else if (iscolor(s.substr(found + 1, 4))) {
			s.erase(found, 5);
		}
		else {
			std::cout << s << std::endl;
			warning("Escape character with no color terminator, output formatting may fail", __LINE__, __FILE__);			warning("Escape character with no color terminator, output formatting may fail", __LINE__, __FILE__);
			exit(1);
			break;
		}
		found = s.find('\033');
	}
	return s;
}
bool iscolor(std::string str) {
	if (str[0] == '[' && str.back() == 'm') {
		str.erase(0,1);
		str.erase(str.end()-1, str.end());
		int code = stof(str);
		if (code == 0 || code == 1 || code == 4 || code == 7 || code == 21 || code == 24 || code == 27) {
			return true;
		}
		else if (30 <= code && code <= 37) {
			return true;
		}
		else if (40 <= code && code <= 47) {
			return true;
		}
	}
}

#ifdef using_hash
void Hash::write(const std::string&	filename) {
	std::ofstream out;
	if (exists(filename)) {
		if (!prompt("File exists, overwrite? (y|n) ")) {
			println("File will not be overwritten, exiting "+cyan+"Hash"+yellow+"::"+bright+white+"write"+res);			println("File will not be overwritten, exiting "+cyan+"Hash"+yellow+"::"+bright+white+"write"+res);
			return;
		}
		else {
			println("Overwriting "+yellow+filename+res);			println("Overwriting "+yellow+filename+res);
		}
	}
	print("Writing file "+yellow+filename+res+"... ");	print("Writing file "+yellow+filename+res+"... ");
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
	println(green+"done"+res);	println(green+"done"+res);
}

Hash* readfile(const std::string& filename) {
	if (!exists(filename)) {
		std::cout << red+white_back << " Error " << black_back << " file does not exist." << std::endl;
		return NULL;
	}
	print("Reading file "+yellow+filename+res+"... ");	print("Reading file "+yellow+filename+res+"... ");
	print("\n", 2);	print("\n", 2);
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
	println("  {minradius}:  "+magenta+bright+std::to_string(minradius)+res+"\n  {maxradius}:  "+		bright+magenta+std::to_string(maxradius)+res+"\n  {blockwidth}: "+bright+magenta+		std::to_string(blockwidth)+res, 3);		std::to_string(blockwidth)+res, 3);
	println("Number of bodies: "+bright+red+std::to_string(nbodies)+res, 3);	println("Number of bodies: "+bright+red+std::to_string(nbodies)+res, 3);
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
			println(line, 3);			println(line, 3);
			n++;
			std::size_t start = line.find("(");
			if (start != std::string::npos) {
				x = stof(line.substr(start+1,line.find(",") - start-1));
				y = stof(line.substr(line.find(",")+2, line.find_last_of(",")-1));
				z = stof(line.substr(line.find_last_of(",")+2,line.find(")")-1));
			}
			else {
				println(red+white_back+" Error "+res+" File error at line "+std::to_string(n)+" in file "+yellow+filename+res);				println(red+white_back+" Error "+res+" File error at line "+std::to_string(n)+" in file "+yellow+filename+res);
				return NULL;
			}
			std::getline(file, line); 
			println(line, 3);			println(line, 3);
			n++;
			radius = stof(line.substr(line.find_last_of(" ")+1));
			std::getline(file, line); 
			println(line, 3);			println(line, 3);
			n++;
			mass = stof(line.substr(line.find_last_of(" ")+1));
			std::getline(file, line); 
			println(line, 3);			println(line, 3);
			n++;
			speed = stof(line.substr(line.find_last_of(" ")+1));
			CBody* body = new CBody(mass, radius, speed, x, y, z);
			body -> Name(name);
			hash -> addNode(body);
			print(body -> info(), 2);			print(body -> info(), 2);
		}
	}
	debug = gdebug;
	println(green+"done"+res);	println(green+"done"+res);
	return hash;
}
#endif // using_hash

std::string CBody::writeFormat(format f) {
	std::string formatted;
	switch(f) {
		case text:
			formatted.append("Body - "+Name()+"\n");
			formatted.append("   Position "+scientific(x,5)+" "+scientific(y,5)+" "+scientific(z,5)+"\n");			formatted.append("   Position "+scientific(x,5)+" "+scientific(y,5)+" "+scientific(z,5)+"\n");
			formatted.append("   Radius   "+scientific(radius)+"\n");			formatted.append("   Radius   "+scientific(radius)+"\n");
			formatted.append("   Mass     "+scientific(mass)+"\n");			formatted.append("   Mass     "+scientific(mass)+"\n");
			formatted.append("   Velocity "+scientific(xv,5)+" "+scientific(yv,5)+" "+scientific(zv,5)+"\n\n");			formatted.append("   Velocity "+scientific(xv,5)+" "+scientific(yv,5)+" "+scientific(zv,5)+"\n\n");
			break;
	}
	print("--------- "+cyan+"Writestream "+res+"--------- \n"+formatted+"------------------------------- \n", 2);	print("--------- "+cyan+"Writestream "+res+"--------- \n"+formatted+"------------------------------- \n", 2);
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
		std::cout << s+"\n";
	}
}
void println(const double& d, int depth) {
	if (debug >= depth && depth != -1) {
		std::cout << std::to_string(d)+"\n";
	}
}
void println(const float& f, int depth) {
	if (debug >= depth && depth != -1) {
		std::cout << std::to_string(f)+"\n";
	}
}
void println(const int& i, int depth) {
	if (debug >= depth && depth != -1) {
		std::cout << std::to_string(i)+"\n";
	}
}

void printr(const std::string& l, const std::string r, int depth) {
	if (debug >= depth && depth != -1) {
		int left = nchar(l);
		int right = nchar(r);
		int termwidth = winwidth();
		std::string vl = l;
		if (termwidth < left + right) {
			println(l+" "+r, depth);			println(l+" "+r, depth);
		}
		else {
			int middle = termwidth - (left + right)-1;
			for (int ii = 0; ii < middle; ii ++) {
				vl = vl+" ";
			}
			std::cout << vl+r;
		}		
	}	
}
void printrln(const std::string& l, const std::string r, int depth) {
	if (debug >= depth && depth != -1) {
		printr(l, r+"\n", depth);		printr(l, r+"\n", depth);
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
