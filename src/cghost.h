#ifndef CGHOST_H
#define CGHOST_H

#include "cbody.h"

class CGhost : public CBody {

public:
	CGhost();
	CGhost(double Mass, double Radius, double Velocity, double X, double Y, double Z, int H=1);

};

#endif
