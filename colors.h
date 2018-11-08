#ifndef COLORS_H
#define COLORS_H

#include <string>

const std::string prefix = "\033[";

const std::string black   = prefix + "30m";
const std::string red	  = prefix + "31m";
const std::string green   = prefix + "32m";
const std::string yellow  = prefix + "33m";
const std::string blue	  = prefix + "34m";
const std::string magenta = prefix + "35m";
const std::string cyan	  = prefix + "36m";
const std::string white   = prefix + "37m";

const std::string black_back   = prefix + "40m";
const std::string red_back     = prefix + "41m";
const std::string green_back   = prefix + "42m";
const std::string yellow_back  = prefix + "43m";
const std::string blue_back    = prefix + "44m";
const std::string magenta_back = prefix + "45m";
const std::string cyan_back    = prefix + "46m";
const std::string white_back   = prefix + "47m";

const std::string res	      = prefix + "0m";
const std::string bright      = prefix + "1m";
const std::string underline   = prefix + "4m";
const std::string inverse     = prefix + "7m";
const std::string unbright    = prefix + "21m";
const std::string deunderline = prefix + "24m";
const std::string uninvert    = prefix + "27m";

#endif
