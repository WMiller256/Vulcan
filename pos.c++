#include "csim.h"

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

int Pos::originDist() {
	return sqrt(pow(x,2) + pow(y,2) + pow(z,2));
}

void Pos::print() {
	std::cout << "(" << bright+magenta << x << res+", "+bright+magenta << y << res+", "+bright+magenta;
	std::cout << z << res+")";
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
