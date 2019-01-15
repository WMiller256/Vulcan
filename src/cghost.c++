#include "cghost.h"

CGhost::CGhost() : CBody() {
	Type(bodyType::ghost);
}
CGhost::CGhost(double Mass, double Radius, double Velocity, double X, double Y, double Z, int H) : CBody(Mass, Radius, Velocity, X, Y, Z, H) {
	Type(bodyType::ghost);
}
        
