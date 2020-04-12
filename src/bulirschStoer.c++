/* bulirschStoer.c++
 *
 * William Miller
 * Apr 7, 2020
 *
 * Implementation for Bulirsch Stoer integrator, 
 * inherited from Integrator base class
 *
 */

#include "bulirschStoer.h"

const int BulirschStoer::nsteps = 12;
const int BulirschStoer::steps[] = { 2,4,6,8,12,16,24,32,48,64,96,128 };
const double BulirschStoer::threshold = 100000;
const double BulirschStoer::grow = 1.3;
const double BulirschStoer::shrink = 0.55;

BulirschStoer::BulirschStoer() {
	init();
}

void BulirschStoer::init() {
	tolerance = 1.0;
	error = std::valarray<double>(0.0, nreal);
	rscale = std::valarray<double>(0.0, nreal);
	vscale = std::valarray<double>(0.0, nreal);
	hc = std::valarray<double>(nreal);
	hs = Matrix<double>(nreal, nsteps);
	h2 = std::valarray<double>(nreal);
	r = std::valarray<Pos>(nreal);
	v = std::valarray<Vel>(nreal);
	rn = std::valarray<Pos>(nreal);
	vn = std::valarray<Vel>(nreal);
	dr = Matrix<Pos>(nreal, nsteps);
	dv = Matrix<Vel>(nreal, nsteps);
}

int BulirschStoer::step() {
	Vel v;
	Pos c;
	vec f;
	Pos p = c;
	int hdid;
}
std::vector<Force> BulirschStoer::gravity() {
/*	
println(in("BulirschStoer", "gravity")+"    Calculating gravitational forces", 4);
	CBody* body;
	std::vector<Force> forces(nreal, Force(0.0, 0.0, 0.0));
	for (int ii = 0; ii < nreal; ii ++) {
		body = read[ii];
		for (auto rb : read[ii]) {
			if (rb != body) {
				printrln("\n"+in("BulirschStoer", "gravity")+"    Target: ", body->Name(), 5); 

				forces[ii] += (body->r - read[ii]->r).unit() * (G * body->Mass() * read[ii]->Mass()) / (read[ii]->squareDistance(body));

				printrln(in("BulirschStoer", "gravity")+"    Magnitude of force between "+body->Name()+" and "+
					read[ii]->Name()+" is ", scientific(G * body->Mass() * read[ii]->Mass()) / (read[ii]->squareDistance(body)), 5);
				printrln(in("BulirschStoer", "gravity")+"    Net force vector on "+body->Name()+" is ", body->net.info(), 5);
			}
		}
	}
	return forces; */
}
vec BulirschStoer::acceleration(Pos r, int idx) {
	vec a(0,0,0);
	for (int ii = 0; ii < nbodies; ii ++) {
		if (ii != idx) {
			// G M / r^2 r-hat
			a += (r - read[ii]->r).unit() * (G * read[ii]->m) / (read[ii]->r - r).squared();
		}
	}
	return a;
}
void BulirschStoer::main(CBody* b, CBody* w) {
	int ii = b->idx;
	rscale[ii] = b->r.norm() > 1e3 ? 1.0 / b->r.norm() : 0.0;
	vscale[ii] = b->v.norm() > 1e3 ? 1.0 / b->v.norm() : 0.0;
	// For each value in {steps}, perform modified midpoint integration with {steps[n]} substeps
mmid:
	vec a = acceleration(b->r, ii);
	for (int n = 1; n < nsteps; n++) {
		hc[ii] = b->h / (2.0 * float(n));
		println(in("BulirschStoer", "main")+"                            MMID on {"+cyan+b->Name()+res+"}, stepsize "+magenta+std::to_string(hc[ii]), 5);
		hs(ii, n) = 0.25 / (n*n);
		h2[ii] = hc[ii] * 2.0;

		// Perform modified midpoint integration with {steps[n]} steps.
		r[ii] = b->r + hc[ii] * b->v;
		v[ii] = b->v + hc[ii] * a;
		a = acceleration(r[ii], ii);
		rn[ii] = r[ii] + h2[ii] * v[ii];
		vn[ii] = v[ii] + h2[ii] * a;
		for (int j = 1; j < n; j ++) {
			a = acceleration(rn[ii], ii);
			r[ii] = r[ii] + h2[ii] * vn[ii];
			v[ii] = v[ii] + h2[ii] * a;
			a = acceleration(r[ii], ii);
			rn[ii] = rn[ii] + h2[ii] * v[ii];
			vn[ii] = vn[ii] + h2[ii] * a;
		}
		a = acceleration(rn[ii], ii);

		// Update the delta matricies (used for polynomial extrapolation)
		dr(ii, n) = 0.5 * (rn[ii] + r[ii] + hc[ii]*vn[ii]);
		dv(ii, n) = 0.5 * (vn[ii] + v[ii] + hc[ii]*a);

		// Perform polynomial extrapolation
		for (int jj = n - 2; jj >= 0; jj --) {
			dr(ii, jj) = (1.0 / (hs(ii, jj) - hs(ii, n))) * hs(ii, jj+1) * dr(ii, jj+1) - (1.0 / (hs(ii, jj) - hs(ii, n-1))) * hs(ii, n-1) * dr(ii, jj);
			dv(ii, jj) = (1.0 / (hs(ii, jj) - hs(ii, n))) * hs(ii, jj+1) * dv(ii, jj+1) - (1.0 / (hs(ii, jj) - hs(ii, n-1))) * hs(ii, n-1) * dv(ii, jj);
		}
		
		// After several integrations, check the relative error for
		// satisfaction of completion condition
		if (n > 3) {
			error[ii] = std::max(dr(ii, 0)*rscale[ii], dv(ii, 0)*vscale[ii], vecComp).norm();
			println(in("BulirschStoer", "main")+"                            Error is "+scientific(error[ii], 5), 5);
			// If error is sufficiently small, update the body position
			if (error[ii] <= tolerance) {
				w->r = dr(ii, 0);
				w->v = dv(ii, 0);
				for (int jj = 0; jj < n; jj ++) {
					w->r += dr(ii, jj);
					w->v += dv(ii, jj);
				}
				// Change h-value depending on the number of steps needed to reach tolerance
				if (n == nsteps) {
					w->h *= shrink;
					b->h = w->h;
				}
				if (n < nsteps && b->h < 1e5) {
					w->h *= grow;
					b->h = w->h;
				}
				return;
			}
		}
	}
	b->h *= 0.5; 		// If completion condition was not met, cut h-value in half
	if (b->h < 1e-5) { 	// Check that h-value is still sufficiently large to continue
		mtx.lock();
		warning("Error at simulation time "+red+std::to_string(simTime)+res, __LINE__, __FILE__);	// Exclude
		warning("Step size scaled below minimum value for body {"+cyan+b->Name()+res+"}"); // Exclude
		warning("h-value is             "+magenta+scientific(b->h, 3)+res+" at position "+b->r.info(3)+" with velocity "+b->v.info(3)); // Exclude
		warning("extrapolation error is "+magenta+scientific(error[ii], 5)+res); // Exclude
		exit(0);
	}
	goto mmid;		// and then try again
}
int BulirschStoer::NSteps() { return nsteps; }

bool vecComp(vec const &l, vec const &r) { return l.norm() < r.norm(); }
