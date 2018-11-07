#ifndef SIMIO_H
#define SIMIO_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#include "csim.h"

Hash::Hash read(const std::string& filename);
bool exists(const std::string& filename);
bool prompt(const std::string& p);


#endif
