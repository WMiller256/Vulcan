#include <iostream>
#include <iomanip>
#include <math.h>
#include "colors.h"
#include <string.h>

const float e = 2.7182818284590452353602874713526624977572;	 
float toDisplay(float in, float def, float n);
float fromDisplay(float in, float def, float n);

int main(int argn, char** argv) {
	if (argn < 3) {
		std::cout << red << "Error not enough arguments" << res << std::endl;
		return -1;
	}
	float n = 100.00;
	float d = 100.00;
	if (strcmp(argv[1],"to") == 0) {
		float r  = atof(argv[2]);
		float dr = toDisplay(r,d,n);
		std::cout << "Radius         " << std::fixed << std::setprecision(2) << r << std::endl;
		std::cout << "Display radius " << std::fixed << std::setprecision(2) << dr << std::endl;
	}
	else if (strcmp(argv[1], "from") == 0) {
		float dr = atof(argv[2]);
		float r  = fromDisplay(dr,d,n);
		std::cout << "Display radius " << std::fixed << std::setprecision(2) << dr << std::endl;
		std::cout << "Radius         " << std::fixed << std::setprecision(2) << r << std::endl;
	}
}

float toDisplay(float in, float def, float n) {
	return log(in / def + 1)*n;
}
float fromDisplay(float in, float def, float n) {
	return (pow(e,in / n) - 1) * def;
}
