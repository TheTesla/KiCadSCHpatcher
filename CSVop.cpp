
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
            if(entrypairs[i].strcontainsentry){
                if(entrypairs[i].fieldentry.find(tab.Tableread(row, col))) cnt++;
            }
            else if(entrypairs[i].entrycontains){
                if(tab.Tableread(row, col).find(entrypairs[i].fieldentry)) cnt++;
            }
            else {
                if(tab.Tableread(row, col)==entrypairs[i].fieldentry) cnt++;
            }
        }
        if(Nbr2find==cnt) break;
        row++;
    }
    return row;
}

int CSVop::findrow(vector<datapair_t> entrypairs, int startrow)
{
    return findrow(entrypairs, startrow, int(entrypairs.size()));
}


string CSVop::getEntry(int row, string fieldname, bool namecontains, bool strcontainsname)
{
    int col;
    col = tab.findcol(fieldname, 0, 0, namecontains, strcontainsname);
    return tab.Tableread(row, col);
}

string CSVop::getfullName(string fieldname, bool namecontains, bool strcontainsname)
{
    int col;
    col = tab.findcol(fieldname, 0, 0, namecontains, strcontainsname);
    return tab.Tableread(0, col);
}

void CSVop::getEntrys(int row, vector<datapair_t> &data2update, bool takeDatabasefieldnames)
{
    int i;
    for(i=0;i<data2update.size();i++){
        data2update[i].fieldentry = getEntry(row, data2update[i].fieldname, data2update[i].namecontains, data2update[i].strcontainsname);
        if(takeDatabasefieldnames) data2update[i].fieldname = getfullName(data2update[i].fieldname, data2update[i].namecontains, data2update[i].strcontainsname);
    }
}
