#include "simio.h"

static const std::vector<int> r = { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
										255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
										255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
										255, 255, 255, 255, 255, 255, 255, 255,249, 244, 239, 234, 229, 224, 
										219, 214, 209, 204, 198, 193, 188, 183, 178, 173, 168, 163, 158, 153, 
										147, 142, 137, 132, 127, 122, 117, 112, 107, 102, 96, 91, 86, 81, 76, 
										71, 66, 61, 56, 51, 45, 40, 35, 30, 25, 20, 15, 10, 5, 0, 0};

static const std::vector<int> g = { 0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 51, 56, 61, 66, 71, 76, 81, 86, 
										91, 96, 102, 107, 112, 117, 122, 127, 132, 137, 142, 147, 153, 158, 
										163, 168, 173, 178, 183, 188, 193, 198, 204, 209, 214, 219, 224, 229, 
										234, 239, 244, 249, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
										255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
										255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
										255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

static const std::vector<int> b = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
										0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
										0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
										0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
										0, 0, 0, 0, 0, 0, 0, 0, 0};


void print_percent(const double &current, const double &total) {
	uint percent = (current / total) * 100;
	if (percent > 100) percent = 100;

	std::string p = std::to_string(percent);
	uint idx = percent - 1 >= 0 ? percent : 0;

	// Print with colormap from red to green
	std::cout << "\r\033[38;2;"+std::to_string(r[idx])+";"+
								std::to_string(g[idx])+";"+
								std::to_string(b[idx])+"m"+
				 std::string(3 - p.length(), '0')+p+"%"+res << std::flush;

	if (percent == 100) std::cout << res+"\n" << std::flush;
}

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
	printf(std::string("\r"+red+white_back+" Error "+res+" [File "+yellow+"%s"+res+" line "+bright+red+"%d"+
		res+"] - "+s+"\n").c_str(), file, line);
}
void warning(const std::string& s, const int line, const char* file) {
	printf(std::string("\r"+bright+blue+white_back+" Warning "+res+" [File "+yellow+"%s"+res+" line "+bright+red+"%d"
		+res+"] - "+s+"\n").c_str(), file, line);
}
void warning(const std::string& s) {
	printf("%s", std::string("\r"+bright+blue+white_back+" Warning "+res+" - "+s+"\n").c_str());
}

std::string scientific(double d, int precision) {
	std::stringstream ret;
	if (d < 0) precision -= 1;
	ret << std::scientific << std::setprecision(precision) << d;
	return ret.str();
}

std::string stripcolors(const std::string& str) {
	std::string s = str;
	int found = s.find('\033');
	while (found != std::string::npos) {
		if (iscolor(s.substr(found+1, 3))) {
			s.erase(found, 4);
		}
		else if (iscolor(s.substr(found+1, 4))) {
			s.erase(found, 5);
		}
		else {
			std::cout << s << std::endl;
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
		str.erase(str.end(), str.end());
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
	return false;
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
		int left = stripcolors(l).size();
		int right = stripcolors(r).size();
		int termwidth = winwidth();
		std::string vl = l;
		if (termwidth < left + right) {
		}
		else {
			while (left % 4) { 
				vl += " ";
				left ++;
			}
			int middle = termwidth - (left + right);
			for (int ii = 0; ii < middle/4; ii ++) { vl += "  - "; }
			std::cout << vl+r;
		}		
	}	
}
void printrln(const std::string& l, const std::string r, int depth) {
	if (debug >= depth && depth != -1) {
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
