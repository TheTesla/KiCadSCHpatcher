
#include "CSVop.h"

CSVop::CSVop()
{

}

CSVop::~CSVop()
{
    iCSVfile.close();
}


int CSVop::readCSVfile(ifstream &file)
{
    if(!file.is_open()) return -1;
    tab.loadTable(file, "\t");
    file.clear();
    file.seekg(0, ios::beg);
    return 0;
}

int CSVop::readCSVfile(string filename)
{
    iCSVfile.open(filename.c_str());
    return readCSVfile(iCSVfile);
}


int CSVop::findrow(string fieldname, string fieldentry, int startrow, bool namecontains, bool entrycontains, bool strcontainsname, bool strcontainsentry, unsigned subpart)
{
    int col, row;
    col = tab.findcol(fieldname, 0, 0, namecontains, strcontainsname);
    row = tab.findrow(fieldentry, col, startrow, entrycontains, strcontainsentry);
    return row;
}

int CSVop::findrow(vector<datapair_t> entrypairs, int startrow, int Nbr2find)
{
    int col, row, i, cnt;
    row = startrow;
    if(0==entrypairs.size()) return -1;
    while(1){
        cnt = 1;
        row = findrow(entrypairs[0].fieldname, entrypairs[0].fieldentry, row, entrypairs[0].entrycontains, entrypairs[0].strcontainsentry);
        if(-1==row) break;
        for(i=1;i<entrypairs.size();i++){
            col = tab.findcol(entrypairs[i].fieldname, 0, 0, entrypairs[i].namecontains, entrypairs[i].strcontainsname);
            if(tab.Tableread(row, col)==entrypairs[i].fieldentry) cnt++;
        }
        if(Nbr2find==cnt) break;
        row++;
    }
    return row;
}

int CSVop::findrow(vector<datapair_t> entrypairs, int startrow, bool namecontains, bool entrycontains, bool strcontainsname, bool strcontainsentry)
{
    return findrow(entrypairs, startrow, int(entrypairs.size()), entrycontains, strcontainsentry);
}


string CSVop::getEntry(int row, string fieldname, bool namecontains, bool strcontainsname)
{
    int col;
    col = tab.findcol(fieldname, namecontains, strcontainsname);
    return tab.Tableread(row, col);
}
