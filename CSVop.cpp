
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

int CSVop::findrow(string fieldname, string fieldentry, int startrow, unsigned subpart)
{
    int col, row;
    col = tab.findcol(fieldname);
    row = tab.findrow(fieldentry, col, startrow);
    return row;
}


//int CSVop::findrow(KiCadStdfn_et fieldname, string fieldentry, int startrow, unsigned subpart)

string CSVop::getEntry(int row, string fieldname)
{
    int col;
    col = tab.findcol(fieldname);
    return tab.Tableread(row, col);
}
