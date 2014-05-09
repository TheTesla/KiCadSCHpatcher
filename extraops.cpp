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

double norm_value(string str, size_t &start)
{
    string valstr;
    valstr = "";
    size_t commapos, endpos;
    double prefix, value;
    size_t prefixpos, emptypos;
    prefix = 1;
    start = str.find_first_of("0123456789.+-", start);
    if(std::string::npos==start) return NAN; // not a number
    commapos = str.find_first_not_of("0123456789", start);
    if(std::string::npos==commapos){
        endpos = std::string::npos;
    }else{
        endpos = str.find_first_not_of("0123456789", commapos+1);
    }
    if(std::string::npos==endpos) {
        valstr = str.substr(start);
    }
    else{
        valstr = str.substr(start, endpos-start);
    }
    if(std::string::npos!=commapos) valstr[commapos-start] = '.';
    if(0==commapos) valstr = "0"+valstr;

    prefixpos = str.find_first_of("afpnµumkKMGTP", start);
    if((prefixpos!=std::string::npos) && (prefixpos>endpos+1)){ // Characters between last number digit
        emptypos = str.find_first_not_of(" _", start); // only spaces under "_" are allowed
        if((emptypos==std::string::npos) || (emptypos!=prefixpos-1)) prefixpos = std::string::npos;
    }
    if(std::string::npos!=prefixpos){
        if('a'==str[prefixpos]) prefix = 0.000000000000000001;
        if('f'==str[prefixpos]) prefix = 0.000000000000001;
        if('p'==str[prefixpos]) prefix = 0.000000000001;
        if('n'==str[prefixpos]) prefix = 0.000000001;
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

    try{
        value = stod(valstr) * prefix;
    }
    catch(const std::invalid_argument& oor){
        std::cerr << "norm_value() - Invalid argument: " << oor.what() << ", argument=" << valstr << "\n";
        return NAN;
    }
    if(stod(""+valstr+"")!=stod(valstr)) cout << "BUG!" << endl;

    start = endpos+1;
    if(std::string::npos==endpos) start = std::string::npos;
    return value;
}

double norm_value(string str)
{
    size_t start = 0;
    return norm_value(str, start);
}

bool entrymatch(string str, string findstr, bool strcontainsentry, bool entrycontains, bool valuesearch, double precision)
{
    std::stringstream ssval;
    double dval;
    ssval.clear();
    ssval.str("");
    ssval << str;
    ssval >> dval;
    if(valuesearch){
        if(strcontainsentry){
            if(dval>norm_value(findstr)) return true;
        }else if(entrycontains){
            if(dval<norm_value(findstr)) return true;
        }else{
            if(precision>abs(norm_value(findstr)-dval)) return true;
        }
    }
    if(strcontainsentry) if(string::npos!=findstr.find(str)) return true;
    if(entrycontains) if(string::npos!=str.find(findstr)) return true;
    if(str==findstr) return true;
    return false;
}

// to search by tolerance
bool entrymatchtol(string str, string valstr, string tolstr, bool upper)
{
    std::stringstream ssstr;
    double dstr;
    ssstr.clear();
    ssstr.str("");
    ssstr << str;
    ssstr >> dstr;

    if(upper){
        if(dstr<valupp(norm_value(valstr),tolupp(tolstr),is_relative(tolstr))) return true;
    }else{
        if(dstr>vallow(norm_value(valstr),tollow(tolstr),is_relative(tolstr))) return true;
    }

    return false;
}

bool checkCONF(vector<datapair_t> searchvec)
{
    unsigned i;
    for(i=0;i<searchvec.size();i++){
        if((searchvec[i].withtolerance) && (searchvec.size() < i+2)){
            return false;
        }
    }
    return true;
}
