#ifndef TOLERANCE_H_INCLUDED
#define TOLERANCE_H_INCLUDED

#include <string>
#include <iostream>       // std::cerr
#include <stdexcept>      // std::out_of_range
#include <cmath>
#include "extraops.h"

typedef enum reltypes_et
{
    ABS = 0,
    REL = 1,
    PERCENT = 2,
    PPM = 3,
    PPB = 4
} reltypes_et;

reltypes_et is_relative(std::string str);
double getfactor(size_t reltype);
double getval(std::string str, size_t &start);
double tolupp(std::string str, double sign = 1);
double tollow(std::string str);
double valupp(double value, double tolupp, reltypes_et reltype);
double vallow(double value, double tollow, reltypes_et reltype);

#endif // TOLERANCE_H_INCLUDED
