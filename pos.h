/*
 * pos.h
 *
 * William Miller
 * Nov 8, 2018
 *
 * Class definition for Pos, a structure to hold a three dimensional position vector
 *
 *
 */

#ifndef POS_H
#define POS_H

class Pos
{
public:
	Pos();
	Pos(double X, double Y, double Z);

	double X();
	double Y();
	double Z();
	void setX(double X);
	void setY(double Y);
	void setZ(double Z);

	double originDist();

	std::string info();
	std::string infoln();

	bool operator==(Pos r) const;

private:
	double x;
	double y;
	double z;

	void init();
};

#endif // POS_H
