
#include "Table.h"
#include <iostream>

Table::Table()
{
    tabstr = nullptr;
}

Table::~Table()
{
    if(nullptr!= tabstr) delete[] tabstr;
}

int Table::loadTable(ifstream &File, string delim, string ignorebefore, string ignoreafter, bool findtabsize)
{
    int col, row;
    size_t pos, oldpos, entrystartpos, qmpos, entrbegpos, entrendpos;
    string entry, line;
    bool in_quotation_marks = false;

    if(!File.is_open()) return -2;

    File.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try{

        col = 0;
        row = 0;
        if(findtabsize){
            rows = 0;
            cols = 0;
        }else{
            tabstr = new string[rows*cols];
        }
        while(1){
            getline(File, line);
            col = 0;
            pos = 0;
            oldpos = 0;
            qmpos = 0;
            entrystartpos = 0;
            in_quotation_marks = false;
            while(line.size()!=pos){
                pos = line.find_first_of(delim, oldpos);
                // Leerzeichen zwischen Anfuehrungszeichen zaehlen nicht
                qmpos = oldpos;
                if(string::npos==pos) pos = line.size();
                while(1){ // weil zwei Anfuehrungszeichen in einem Eintrag sein koennen
                    qmpos = line.find_first_of("\"", qmpos);
                    if((string::npos==qmpos)||(qmpos>pos)) break;
                    qmpos++;
                    in_quotation_marks = !in_quotation_marks;
                }
                if(!in_quotation_marks){
                    // zwischen zwei Delimiter befindet sich der Eintrag
                    if((oldpos<pos)||((string::npos==ignorebefore.find(delim))&&(string::npos==ignoreafter.find(delim)))){
                            /* Besonderheit: Delimitterzeichen ist zu ignorierendes Zeichen -
                                             keine Unterscheidung zwischen leeren Eintraegen
                                             und mehreren zu ignorierenden Fuellzeichen moeglich
                            */

                        entry = line.substr(entrystartpos, pos-entrystartpos);
                        entrbegpos = entry.find_first_not_of(ignorebefore); // zu ignorierende Zeichen am Anfang, ...
                        entrendpos = entry.find_last_not_of(ignoreafter); // zu ignorierende Zeichen am Ende, ...
                        if((string::npos!=entrbegpos)&&(string::npos!=entrendpos)){
                            entry = entry.substr(entrbegpos, entrendpos-entrbegpos+1); // ... entfernen
                        }else{
                            entry = "";
                        }
                        if(findtabsize){
                            cols = max(cols, col);
                        }else{
                            tabstr[col+row*cols] = entry;
                        }
                        col++;
                    }
                    entrystartpos = pos+1; // weil oldpos auch innerhalb der Anfuehrungszeichen unterteilt
                }
                oldpos = pos+1;
            }
            row++;
        }
    }


    catch(std::ifstream::failure e){
        if(File.bad()) return -11;
        if(File.eof()) {
            File.clear();
            File.seekg(0, ios::beg);
            if(findtabsize){
                rows = row;
                cols++; // Korrektur: Spalte 0 ist auch eine Spalte; cols muss um eins groesser sein als letzte Spaltennummer
                loadTable(File, delim, ignorebefore, ignoreafter, false);
            }
            return 0;
        }
        return -1;
    }
    return -255; // nicht erreichbar
}

string Table::Tableread(int row, int col)
{
    OK = false;
    if(row>=rows || col>=cols || row<0 || col<0) return "";
    OK = true;
    return tabstr[col + row*cols];
}

int Table::Tablewrite(int row, int col, string entry)
{
    OK = false;
    if(row>=rows || col>=cols || row<0 || col<0) return -1;
    OK = true;
    tabstr[col+row*cols] = entry;
    return 0;
}

int Table::findrow(string findstr, int col, int firstrow, bool entrycontains, bool strcontainsentr, bool valuesearch, double precision)
{
    int row;
    for(row=firstrow;row<rows;row++){
        if(entrymatch(Tableread(row,col),findstr,strcontainsentr,entrycontains,valuesearch,precision)) return row;
    }
    return -1;
}

int Table::findrow_bw(string findstr, int col, int lastrow, bool entrycontains, bool strcontainsentr)
{
    int row;
    string str;
    for(row=lastrow;row>=0;row--){
        str = Tableread(row,col);
        if(strcontainsentr) if(string::npos!=findstr.find(str)) return row;
        if(entrycontains) if(string::npos!=str.find(findstr)) return row;
        if(str==findstr) return row;
    }
    return -1;
}

int Table::findcol(string findstr, int row, int firstcol, bool entrycontains, bool strcontainsentr)
{
    int col;
    string str;
    for(col=firstcol;col<cols;col++){
        str = Tableread(row,col);
        if(strcontainsentr) if(string::npos!=findstr.find(str)) return col;
        if(entrycontains) if(string::npos!=str.find(findstr)) return col;
        if(str==findstr) return col;
    }
    return -1;
}

void Table::eraseemptyrows(void)
{
    int rrow, wrow, col;
    bool colempty;
    wrow = 0;
    string str;
    for(rrow=0;rrow<rows;rrow++){
        colempty = true;
        for(col=0;col<cols;col++){
            str = Tableread(rrow, col);
            tabstr[col + wrow*cols] = str;
            if(0 != str.compare("")) colempty = false;
        }
        if(false == colempty) wrow++;
    }
    rows = wrow;
}

void Table::rmquotmarks(void)
{
    int rowit, colit;
    for(rowit=0;rowit<rows;rowit++){
        for(colit=0;colit<cols;colit++){
            Tablewrite(rowit, colit, ::rmquotmarks(Tableread(rowit, colit)));
        }
    }
}
