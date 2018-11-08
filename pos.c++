#include "csim.h"
#include "simio.h"
Pos::Pos() {
	init();
}
Pos::Pos(double X, double Y, double Z) {
	x = X;
	y = Y;
	z = Z;
}

double Pos::X() {
	return x;
}
double Pos::Y() {
	return y;
}
double Pos::Z() {
	return z;
}
void Pos::setX(double X) {
	x = X;
}
void Pos::setY(double Y) {
	y = Y;
}
void Pos::setZ(double Z) {
	z = Z;
}

double Pos::originDist() {
	double dist = sqrt(pow(x,2) + pow(y,2) + pow(z,2));
	print("In ["+bright+blue+"Pos::originDist()"+res+"]   ", 4);
	print(info(), 4);
	print(" ", 4);
	println(std::to_string(dist), 4);
	if (dist < minradius) {
		minradius = dist;
		blockwidth = (maxradius - minradius) / (nbodies - 1);
		println("                         {maxradius}:  "+bright+magenta+std::to_string(maxradius)+res+
			    "\n                         {minradius}:  "+bright+magenta+std::to_string(minradius)+res+
			    "\n                         {nbodies}:    "+bright+magenta+std::to_string(nbodies)+res+
			    "\n                         {blockwidth}: "+bright+magenta+std::to_string(blockwidth)+res, 5);
		if (blockwidth <= 0) {
			blockwidth = 1.0;
		}
	}
	if (dist > maxradius) {
		maxradius = dist;
		blockwidth = (maxradius - minradius) / (nbodies - 1);
		blockwidth = (maxradius - minradius) / (nbodies - 1);
		println("                         {maxradius}:  "+bright+magenta+std::to_string(maxradius)+res+
			    "\n                         {minradius}:  "+bright+magenta+std::to_string(minradius)+res+
			    "\n                         {nbodies}:    "+bright+magenta+std::to_string(nbodies)+res+
			    "\n                         {blockwidth}: "+bright+magenta+std::to_string(blockwidth)+res, 5);
		if (blockwidth <= 0) {
			blockwidth = 1.0;
		}
	}

	return dist;
}

std::string Pos::info() {
	std::string s = "("+bright+magenta+std::to_string(x)+res+", "+bright+magenta;
	s += std::to_string(y)+res+", "+bright+magenta+std::to_string(z)+res+")";
	return s;
}

bool Pos::operator==(Pos r) const {
	if (x == r.X() && y == r.Y() && z == r.Z()) {
		return true;
	}
	else {
		return false;
	}
}

void Pos::init() {
	x = 0;
	y = 0;
	z = 0;
}
