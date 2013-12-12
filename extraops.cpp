#include "extraops.h"
#include <iostream>

#define COPYRIGHT "Copyright (C) 2013 Stefan Helmert"

string rmquotmarks(string str)
{
    size_t spos, epos;
    spos = str.find_first_not_of("\"");
    epos = str.find_last_not_of("\"");
    if(spos!=string::npos&&epos!=string::npos) str = str.substr(spos, epos-spos+1);
    return str;
}

void rmquotmarks(vector<datapair_t> &data)
{
    unsigned i;
    for(i=0;i<data.size();i++){
        data[i].fieldentry = rmquotmarks(data[i].fieldentry);
    }
}

double norm_value(string str)
{
    string valstr;
    valstr = "";
    size_t commapos, endpos;
    double prefix, value;
    size_t prefixpos;
    prefix = 1;
    if(0==str.size()) return 0;
    commapos = str.find_first_not_of("0123456789");
    endpos = str.find_first_not_of("0123456789", commapos+1);
    if(std::string::npos==endpos) endpos = commapos;
    valstr = str.substr(0, endpos);
    if(std::string::npos!=commapos) valstr[commapos] = '.';
    if(0==commapos) valstr = "0"+valstr;

    prefixpos = str.find_first_of("afpnµumkKMGTP");
    if(std::string::npos!=prefixpos){
        if('a'==str[prefixpos]) prefix = 0.000000000000000001;
        if('f'==str[prefixpos]) prefix = 0.000000000000001;
        if('n'==str[prefixpos]) prefix = 0.000000000001;
        if('p'==str[prefixpos]) prefix = 0.000000001;
        if('µ'==str[prefixpos]) prefix = 0.000001;
        if('u'==str[prefixpos]) prefix = 0.000001;
        if('m'==str[prefixpos]) prefix = 0.001;
        if('k'==str[prefixpos]) prefix = 1000;
        if('K'==str[prefixpos]) prefix = 1000;
        if('M'==str[prefixpos]) prefix = 1000000;
        if('G'==str[prefixpos]) prefix = 1000000000;
        if('T'==str[prefixpos]) prefix = 1000000000000;
        if('P'==str[prefixpos]) prefix = 1000000000000000;
    }

    value = stod(""+valstr+"") * prefix; // workarround gcc bug
    return value;
}

