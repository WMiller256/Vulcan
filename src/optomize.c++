/*
 * optomize.c++
 *
 * William Miller
 * Nov 11, 2018
 *
 * Creates an optomized version of the given .c++ files in the 
 * Sim buildchain by disabling all the debugging lines.
 *
 *
 */
 
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <string.h>
#include "colors.h"

bool exists(const std::string& filename);

int main(int argn, char** argv) {
	std::vector<std::string> filenames;
	std::string name;
	bool verbose;
	for (int ii = 1; ii < argn; ii ++) {
		name = argv[ii];
		if (strcmp(argv[ii], "verbose") == 0) {
			verbose = true;
		}
		else if (strcmp(argv[ii], __FILE__) != 0 && 
			name.find("opt_") == std::string::npos &&
			strcmp(argv[ii], "simio.c++") != 0 &&
			strcmp(argv[ii], "vec.c++") != 0) {
			if (exists(std::string(argv[ii]))) {
				filenames.push_back(argv[ii]);
			}
		}
	}
	std::ifstream file;
	std::ofstream out;
	std::string line;
	std::string comment = "/";
	comment += "/";
	for (int ii = 0; ii < filenames.size(); ii ++) {
		std::cout << yellow+filenames.at(ii)+res+"...";
		file.open(filenames.at(ii));
		out.open("opt_"+filenames.at(ii));
		while (std::getline(file, line)) {
			if (verbose) std::cout << line;
			if (line.find("print(") != std::string::npos || 
				line.find("printr(") != std::string::npos ||
				line.find("println(") != std::string::npos ||
				line.find("printrln(") != std::string::npos ||
				line.find("error(") != std::string::npos ||
				line.find("warning(") != std::string::npos ||
				line.find("scientific(") != std::string::npos) {
				if (line.find("void print") == std::string::npos &&
					line.find("string scientific") == std::string::npos &&
					line.find("void error") == std::string::npos && 
					line.find("void warning") == std::string::npos) {
					line.insert(0, comment);
					out << line;
					while (line.find(";") == std::string::npos) {
						std::getline(file, line);
						line.insert(0, comment);
						out << line;
					}
				}
			}
			if (verbose) std::cout << std::endl;
			out << line << std::endl;
			line = "";
		}
		file.close();
		out.close();
		std::cout << green+" done"+res << std::endl;
	}	
}

bool exists(const std::string& filename) {
	return std::experimental::filesystem::exists(filename);
}
