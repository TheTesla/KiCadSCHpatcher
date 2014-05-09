
#include "tolerance.h"

reltypes_et is_relative(std::string str)
{
    if(std::string::npos!=str.find("ppm")) return PPM;
    if(std::string::npos!=str.find("ppb")) return PPB;
    if(std::string::npos!=str.find("%")) return PERCENT;
    //if(std::string::npos!=str.find("‰")) return 4;
    if(std::string::npos==str.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ°")) return REL;

    return ABS;
}

double getval(std::string str, size_t &start)
{
    size_t strstart, strend;
    std::string valstr;
    double val;
    strstart = str.find_first_of("0123456789.+-",start);
    strend = str.find_first_not_of("0123456789.", strstart+1);
    start = strend + 1;
    if(std::string::npos==strstart) return NAN;
    if(std::string::npos==strend) {
        valstr = str.substr(strstart);
        start = std::string::npos;
    }else{
        valstr = str.substr(strstart,strend-strstart);
    }
    try {
        val = stod(valstr);
    }
    catch(const std::invalid_argument& oor){
        std::cout << valstr << std::endl;
        std::cerr << "getval() - Invalid argument: " << oor.what() << '\n';
        return 0;
    }
    return val;
}

double getfactor(size_t reltype)
{
    switch(reltype){
    case ABS:
        return 0;
    case REL:
        return 1;
    case PERCENT:
        return 0.01;
    case PPM:
        return 0.000001;
    case PPB:
        return 0.000000001;
    }
    return 0;
}

double tolupp(std::string str, double sign)
{
    size_t start = 0;
    reltypes_et reltype;
    double val1, val2, factor;
    reltype = is_relative(str);
    if(ABS==reltype){
        // absolute tolerance value
        // first number
        val1 = norm_value(str,start);
        //second number
        val2 = norm_value(str,start);
        if(isnan(val2)) return sign*abs(val1); // if only one value - negative lower bound
        if(sign*val2>sign*val1) return val2;
        return val1;
    }else{
        //relative tolerance value
        factor = getfactor(reltype);
        // first number
        val1 = getval(str,start) * factor;
        //second number
        val2 = getval(str,start) * factor;
        if(isnan(val2)) return sign*abs(val1); // if only one value - negative lower bound
        if(sign*val2>sign*val1) return val2;
        return val1;
    }
}

double tollow(std::string str)
{
    return tolupp(str,-1);
}

double valupp(double value, double tolupp, reltypes_et reltype)
{
    if(ABS==reltype) return value + tolupp;
    return value * (1+tolupp);
}

double vallow(double value, double tollow, reltypes_et reltype)
{
    return valupp(value, tollow, reltype); // alias - same implementation
}

