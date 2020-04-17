/* integrator.c++
 *
 * William Miller
 * Apr 7, 2020
 *
 * Implementation of integrator base class
 *
 */

#include "csim.h"

std::vector<CBody*> Integrator::one;
std::vector<CBody*> Integrator::two;
std::vector<CBody*> Integrator::read;
std::vector<CBody*> Integrator::write;
double Integrator::h = -1.0;
int Integrator::nghosts = 0;
int Integrator::nbodies = 0;
int Integrator::nreal = 0;

Integrator::Integrator() {}

void Integrator::main(CBody* body, CBody* wbody) {
	println(in("Integrator", "main")+"    Calculating net acceleration on {"+cyan+body->Name()+res+"}", 4);
	wbody->a.zero();
	for (int ii = 0; ii < nreal; ii ++) {
		if (read[ii] != body) {
			printrln("\n"+in("Integrator", "main")+"    Target: ", body->Name(), 5); 
			wbody->a += body->r.direction(read[ii]->r) * (G * read[ii]->Mass()) / read[ii]->squareDistance(body->r);

			printrln(in("Integrator", "main")+"    Magnitude of force between "+body->Name()+" and "+
				read[ii]->Name()+" is ", scientific(G * body->Mass() * read[ii]->Mass() / read[ii]->squareDistance(body->r)), 5);
		}
	}
	// Apply acceleration to velocity
    wbody->v = body->v + wbody->a * h;
    // Apply acceleration to position, extra factor of two comes from applying acceleration to endpoint as well 
    // as initial position. This significantly mitigates temporal degeneracy of the integration accuracy
    wbody->r = body->r + (body->v + wbody->a * h) * h;
    // Update the fix time of the body
    wbody->fix = simTime;
    wbody->ncalcs++;
	printrln(in("Integrator", "main")+"          Acceleration vector on "+bright+red+body->Name()+res+" is ", wbody->a.info(), 5);
	println(in("Integrator", "main")+green+"          Done"+res, 4);

}

void Integrator::set(int b, int g) {
	nbodies = b;
	nghosts = g;
	nreal = b + g;
}
