/* miller.h
 *
 * William Miller
 * Apr 7, 2020
 *
 * Miller integrator class
 *
 */

#ifndef MILLER_H
#define MILLER_H

#include "integrator.h"
 
class Miller : public Integrator {

public:
	Miller();
	void main() override;		// Main integration function
};

#endif // MILLER_H
