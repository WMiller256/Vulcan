#include "vulcan.h"
#include <chrono>

int main(int argn, char** argv) {
	double t = 8.64e6;
	double h = 8.64e5;

	CSim* tsim = new CSim(2, t, h);
	tsim->pyinit();
	tsim->setDebug(0);
	tsim->Type(simType::bulirschstoer);
	tsim->outputInterval(h);

	CBody* earth1 = new CBody(5.97237e24, 6.371e6, -2.978e4, 0.0, -1.521e11, 0.0, h);
	CBody* earth2 = new CBody(5.97237e24, 6.371e6, 2.978e4, 0.0, 1.521e11, 0.0, h);
	earth1->Name("Earth1"); 
	earth2->Name("Earth2");
	tsim->addPlanet(earth1);
	tsim->addPlanet(earth2);
	
	std::cout << green << " Initialization complete. " << res << std::endl;

    tsim->sim();

    std::cout << green << " Simulation complete. " << res << std::endl;
}
