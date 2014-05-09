
#include "CSVop.h"

#include <iostream>

CSVop::CSVop()
{

}

CSVop::~CSVop()
{
    if(iCSVfile.is_open()) iCSVfile.close();
}


int CSVop::readCSVfile(ifstream &file)
{
    int err;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    if(!file.is_open()) return -1;
    err = tab.loadTable(file, CSVparams.delim, CSVparams.ignorebefore, CSVparams.ignoreafter);
    if(0!=err) return err;
    tab.rmquotmarks();
    return 0;
}

int CSVop::readCSVfile(string filename)
{
    iCSVfile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try{
        iCSVfile.open(filename.c_str());
    }
    catch(std::ifstream::failure e){
        return -1;
    }
    return readCSVfile(iCSVfile);
}

int CSVop::readCSVfile(void)
{
    iCSVfile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try{
        iCSVfile.open(CSVparams.filename.c_str());
    }
    catch(std::ifstream::failure e){
        return -1;
    }
    return readCSVfile(iCSVfile);
}

int CSVop::findrow(string fieldname, string fieldentry, int startrow, bool namecontains, bool entrycontains, bool strcontainsname, bool strcontainsentry, bool valuesearch, bool withtolerance, unsigned subpart, double precision)
{
    int col, row;
    col = tab.findcol(fieldname, 0, 0, namecontains, strcontainsname);
    if(withtolerance){
        if(0==col){
            cout << "error: Not a value with Tolerance entry, because it is on most left column!" << endl;
        }else{
            col--;
        }
    }
    row = tab.findrow(fieldentry, col, startrow, entrycontains, strcontainsentry, valuesearch, precision);
    return row;
}

// first match
// Nbr2find (1 - or; number of search entries - and)
int CSVop::findrow(vector<datapair_t> entrypairs, int startrow, int Nbr2find)
{
    int col, colouve0, row, cnt, valcol;
    unsigned i;
    row = startrow;
    if(0==entrypairs.size()) return -1;

    // outside for optimisation only
    colouve0 = tab.findcol(entrypairs[0].fieldname, 0, 0, entrypairs[0].namecontains, entrypairs[0].strcontainsname);
    for(row=startrow;row<tab.getNorows();row++){
        cnt = 0;
        for(i=0;i<entrypairs.size();i++){
            if(0==i){
                col = colouve0;
                if(entrypairs[i].withtolerance){
                    valcol = col;
                    if(entrypairs.size() < i+2){
                        cout << "Error: value with tolerance specified but no search entry in configuration file for tolerance." << endl;
                        break;
                    }
                }

            }else{
                if(entrypairs[i].withtolerance){
                    if(entrypairs.size() < i+2){
                        cout << "Error: value with tolerance specified but no search entry in configuration file for tolerance." << endl;
                        break;
                    }
                    valcol = tab.findcol(entrypairs[i].fieldname, 0, 0, entrypairs[i].namecontains, entrypairs[i].strcontainsname);
                }else if((i>0)&&(entrypairs[i-1].withtolerance)){
                    valcol = tab.findcol(entrypairs[i-1].fieldname, 0, 0, entrypairs[i-1].namecontains, entrypairs[i-1].strcontainsname);
                }else{
                    col = tab.findcol(entrypairs[i].fieldname, 0, 0, entrypairs[i].namecontains, entrypairs[i].strcontainsname);
                }
            }
            // this is a bit "special" - value and tolerance are two parameters, upper and lower bound of value are also two parameters

            if(entrypairs[i].withtolerance){
                // our "cursor" is at first on the value, that means the first parameter
                // but we read also the second parameter (i+1), the tolerance
                // and we are searching for the lower bound
                if(0==valcol){
                    cout << "Error: value column is first one in table - there is no lower bound value on the left, tolerance operation failed." << endl;
                    break;
                }
                if(entrymatchtol(tab.Tableread(row, valcol-1),entrypairs[i].fieldentry,entrypairs[i+1].fieldentry,false)) cnt++;
                else break;
            }else{// a normal parameter does not have the tolerance flag set, same is the tolerance of the value
                // our "cursor" is on the second parameter, that means the second parameter
                // but we read also the first parameter again (i-1), the value
                // and we are searching for the upper bound

                if((i>0) && (entrypairs[i-1].withtolerance)){
                    if(tab.getNocols() < valcol+2){
                        cout << "Error: value column is last one in table - there is no upper bound value on the right, tolerance operation failed." << endl;
                        break;
                    }
                    if(entrymatchtol(tab.Tableread(row, valcol+1),entrypairs[i-1].fieldentry,entrypairs[i].fieldentry,true)) cnt++;
                    else break;
                }else{
                    if(entrymatch(tab.Tableread(row, col),entrypairs[i].fieldentry,entrypairs[i].strcontainsentry,entrypairs[i].entrycontains,entrypairs[i].valuesearch,entrypairs[i].precision)) cnt++;
                    else break;
                }

            }

        }
        if(Nbr2find==cnt) return row;
    }
    return -1;
}

// search and
int CSVop::findrow(vector<datapair_t> entrypairs, int startrow)
{
    return findrow(entrypairs, startrow, int(entrypairs.size()));
}

// all matches
vector<int> CSVop::findrows(vector<datapair_t> entrypairs, int startrow, int Nbr2find)
{
    vector<int> rows;
    int row = 0;
    rows.clear();
    row = startrow-1;
    while(-1!=row){
        row = findrow(entrypairs,row+1,Nbr2find);
        rows.push_back(row);
    }
    rows.pop_back();
    return rows;
}

vector<int> CSVop::findrows(vector<datapair_t> entrypairs, int startrow)
{
    return findrows(entrypairs, startrow, int(entrypairs.size()));
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

void CSVop::getEntrys(int row, vector<datapair_t> &data2update)
{
    unsigned i;
    for(i=0;i<data2update.size();i++){
        data2update[i].fieldentry = getEntry(row, data2update[i].fieldname, data2update[i].namecontains, data2update[i].strcontainsname);
        if(data2update[i].takeDatabasefieldname) data2update[i].fieldname = getfullName(data2update[i].fieldname, data2update[i].namecontains, data2update[i].strcontainsname);
    }
}

int CSVop::getNorows(void)
{
    return tab.getNorows();
}
