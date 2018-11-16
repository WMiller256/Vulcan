/*
 * baseio.c++
 *
 * William Miller
 * Nov 8, 2018
 * 
 * Functions for basic IO operations
 *
 *
 *
 */ 

#include "baseio.h"

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
			warning("Escape character with no color terminator, output formatting may fail", __LINE__, __FILE__);
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
