#include "simio.h"

void Hash::write(const std::string&	filename) {
	std::ofstream out;
	if (exists(filename)) {
		if (!prompt("File exists, overwrite? (y|n) ")) {
			return;
		}
	}
	outfile.open(filename, std::ios::app);
	nodeptr current = new node;
	for (int ii = 0; ii < size; ii ++) {
		current = table[ii];
		if (current != NULL) {
			int depth = current -> bodies.size();
			for (int kk = 0; kk < depth; kk ++) {
				outfile << bodies.at(kk) -> writeFormat();
			}
		}
	}
}

Hash::Hash* read(const std::string& filename) {
	if (!exists(filename)) {
		std::cout << red+white_back << " Error " << black_back << " file does not exist." << std::endl;
		return NULL;
	}
}

std::string CBody::writeFormat(format f) {
	switch(f) {
		case text:
			std::string formatted;
			formatted.append("Body\n");
			formatted.append("\t Position ("+std::to_string(x)+","+std::to_string(y)+", "+std::to_string(z)+")\n");
			formatted.append("\t Radius   "+std::to_string(radius)+"\n";
			formatted.append("\t Mass     "+std::to_string(mass)+"\n";
			formatted.append("\t Speed    "+std::to_string(speed)+"\n\n";
			break;
	}
}

bool exists(const std::string& filename) {
	return std::filesystem::exists(filename);
}

bool prompt(const std::string& p) {
	char in;
	std::cout << p << std::flush;
	std::cin >> in;
	if (in == 'y') {
		return true;
	}
	else {
		return false;
	}
}
