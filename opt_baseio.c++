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
	std::string str = std::to_string(d);
	std::string ret = "";
	if (d == std::numeric_limits<double>::infinity()) {
		return magenta+"inf"+res;
	}
	if (fabs(d) <= 1.0e-9) {
		d = 0.0;
	}
	if (fabs(d) >= 1.0) {
		int dec = str.find(".");
		int start = 1;
		if (dec != std::string::npos) {
			ret += magenta;
			ret += str[0];
			if (d < 0.0) {
				ret += str[1];
				start = 2;
			}
			ret += ".";
	 		if (precision > str.length()) {
				precision = str.length();
			}
			for (int ii = start; ii < precision+start; ii ++) {
				if (str[ii] != '.') {
					ret += str[ii];
				}
				else {
					precision += 1;
				}
			}
			ret += res;
			ret += "e";
			ret += bright+red;
			ret += std::to_string(dec - start);
		}
		else {
			ret = red+"err"+res;
		}
	}
	else if (fabs(d) == 0.0) {
		ret += magenta;
		ret = "0.";
		for (int ii = 0; ii < precision; ii ++) {
			ret += "0";
		}
		ret += res;
		ret += "e";
		ret += bright+red+"0"+res;
	}
	else {
		ret += magenta;
		int dec = str.find(".");
		while (str[dec++] == '0') {
			if (dec+1 == str.length()) {
				warning(" Failed to format given number in scientific notation", __LINE__, __FILE__);
				break;
			}
		}
		if (d < 0.0) {
			ret += "-";
		}
		if (dec != std::string::npos) {
			ret += str[dec];
			ret += ".";
			for (int ii = dec; ii < precision+dec; ii ++) {
				ret += str[ii];
			}
			ret += res;
			ret += "e-";
			ret += bright+red;
			if (d < 0.0) {
				ret += std::to_string(dec+2);
			}
			else {
				ret += std::to_string(dec+1);
			} 
			ret += res;
		}
		else {
			ret = red+"err"+res;
		}
	}
	ret += res;
	return ret;
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
