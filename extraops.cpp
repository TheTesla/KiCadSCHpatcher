#include "extraops.h"

string rmquotmarks(string str)
{
    int spos, epos;
    spos = str.find_first_not_of("\"");
    epos = str.find_last_not_of("\"");
    if(spos!=string::npos&&epos!=string::npos) str = str.substr(spos, epos-spos+1);
    return str;
}

void rmquotmarks(vector<datapair_t> &data)
{
    int i;
    for(i=0;i<data.size();i++){
        data[i].fieldentry = rmquotmarks(data[i].fieldentry);
    }
}
