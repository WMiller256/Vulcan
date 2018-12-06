#define SOLARSYSTEMCONFIG
#include "simio.h"

int main(int argn, char** argv) {
	CSim sim(8);
	CBody* sun = new CBody(1.989e30, 6.95508e8, 0.0, 0.0, 0.0, 0.0);
	CBody* mercury = new CBody(3.3011e23, 2.439e6, 4.7362e4, 6.98169e11, 0.0, 0.0);
	CBody* venus = new CBody(4.8675e24, 6.0518e6, 3.502e4, 1.08939e11, 0.0, 0.0);
	CBody* earth = new CBody(5.97237e24, 6.371e6, 2.978e4, 1.521e11, 0.0, 0.0);
	CBody* mars = new CBody(6.4171e23, 3.3895e6, 2.4007e4, 2.492e11, 0.0, 0.0);
	CBody* jupiter = new CBody(1.8982e27, 6.991e7, 1.307e4, 8.1662e11, 0.0, 0.0);
	CBody* saturn = new CBody(5.683e26, 5.8232e7, 9.68e3, 1.5155e12, 0.0, 0.0);
	CBody* uranus = new CBody(8.681e25, 2.362e7, 6.8e3, 3.008e12, 0.0, 0.0);
	CBody* neptune = new CBody(1.024e26, 2.431e7, 5.43e3, 4.54e12, 0.0, 0.0);

	sun -> Name("Sun"); 
	mercury -> Name("Mercury");
	venus -> Name("Venus");
	earth -> Name("Earth");
	mars -> Name("Mars");
	jupiter -> Name("Jupiter");
	saturn -> Name("Saturn");
	uranus -> Name("Uranus");
	neptune -> Name("Neptune");

	sim.addBody(sun);
	sim.addBody(mercury);
	sim.addBody(venus);
	sim.addBody(earth);
	sim.addBody(mars);
	sim.addBody(jupiter);
	sim.addBody(saturn);
	sim.addBody(uranus);
	sim.addBody(neptune);

	sim.writeConfiguration("../data/solarSystem.sim");
	CSim* check = sim.readConfiguration("../data/solarSystem.sim");

	if (check != NULL) {
		print_special("Generated file "+yellow+"data/solarSystem.sim"+green+" successfully "+res, 'w', 'k');
	}
	else {
		error("Failed to create "+yellow+"data/solarsystem.sim "+res+" - "+bright+red+"NULL"+res+" returned from "+in("CSim","writeConfiguration"), __LINE__, __FILE__);		
	}

	return 0;
}
