/* Klasse verwaltet string-Tabellen
 *
 * Copyright (C) 2012 Stefan Helmert <stefan.helmert@gmx.net>
 *
 *
 * loadTable() liest Datensatz aus Textdatei ein, Komma als Spaltentrenner
 * Leerzeichen am Anfang und Ende eines Eintrags werden entfernt
 */

#include "Table.h"
#include <iostream>

Table::Table()
{
}

Table::~Table()
{
}

void Table::loadTable(ifstream &File)
{
    int col, row, i, pos, oldpos, entrystartpos, qmpos;
    string entry, line;
    bool in_quotation_marks = false;

    findtablesize(File); // wegen dynamischer Speicherverwaltung
    tabstr = new string[rows*cols];
    col = 0;
    row = 0;
    if(File.is_open()){
        while(File.good()){
            getline(File, line);
            col = 0;
            pos = 0;
            oldpos = 0;
            qmpos = 0;
            entrystartpos = 0;
            in_quotation_marks = false;
            while(1){
                pos = line.find_first_of(" ", oldpos);
                // Leerzeichen zwischen Anfuehrungszeichen zaehlen nicht
                qmpos = oldpos;
                while(1){ // weil zwei Anfuehrungszeichen in einem Eintrag sein koennen
                    qmpos = line.find_first_of("\"", qmpos);
                    if((string::npos==qmpos)||(qmpos>pos)) break;
                    qmpos++;
                    in_quotation_marks = !in_quotation_marks;
                }
                if(!in_quotation_marks){
                    if(oldpos<pos){
                        entry = line.substr(entrystartpos, pos-entrystartpos);
                        tabstr[col+row*cols] = entry;
                        col++;
                    }
                    entrystartpos = pos+1; // weil oldpos auch innerhalb der Anfuehrungszeichen unterteilt
                }
                if(string::npos==pos) break;
                oldpos = pos+1;
            }
            entry = line.substr(entrystartpos, line.size()-entrystartpos);
            tabstr[col+row*cols] = entry;
            row++;
        }
    }
}

string Table::Tableread(int row, int col)
{
    if(row>rows || col>cols || row<0 || col<0) return "";
    return tabstr[col + row*cols];
}


// Trennzeichen ist Leerzeichen, zwischen Anfuehrungszeuchen nicht mitgezaehlt
void Table::findtablesize(ifstream &File)
{
    string line;
    size_t pos, oldpos, qmpos;
    bool in_quotation_marks = false;
    rows = 0;
    cols = 0;
    int tmp;
    if(File.is_open()){
        while(File.good()){
            getline(File, line);
            rows++;
            tmp = 0;
            pos = 0;
            oldpos = 0;
            qmpos = 0;
            in_quotation_marks = false;
            while(string::npos!=pos){
                pos = line.find_first_of(" ", oldpos);
                // Leerzeichen zwischen Anfuehrungszeichen zaehlen nicht
                qmpos = oldpos;
                while(1){ // weil zwei Anfuehrungszeichen in einem Eintrag sein koennen
                    qmpos = line.find_first_of("\"", qmpos);
                    if((string::npos==qmpos)||(qmpos>pos)) break;
                    qmpos++;
                    in_quotation_marks = !in_quotation_marks;
                }
                if(!in_quotation_marks){
                    if(oldpos<pos) tmp++;
                }
                oldpos = pos+1;
            }
            if(tmp>cols) cols = tmp;
        }
    File.clear();
    File.seekg(0,ios::beg);
    }
}

int Table::findrow(string find, int col, int firstrow)
{
    int row;
    string str;
    for(row=firstrow;row<rows;row++){
        str = this->Tableread(row,col);
        if(0 == str.compare(find)) return row;
    }
    return -1;
}

int Table::findrow_bw(string find, int col, int lastrow)
{
    int row;
    string str;
    for(row=lastrow;row>=0;row--){
        str = this->Tableread(row,col);
        if(0 == str.compare(find)) return row;
    }
    return -1;
}

int Table::findcol(string find, int row, int firstcol)
{
    int col;
    string str;
    for(col=firstcol;col<cols;col++){
        str = this->Tableread(row,col);
        if(0 == str.compare(find)) return col;
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
            str = this->Tableread(rrow, col);
            tabstr[col + wrow*cols] = str;
            if(0 != str.compare("")) colempty = false;
        }
        if(false == colempty) wrow++;
    }
}

int Table::findKiCadSCHrow(string fieldname, string fieldentry)
{
    int row;
    row = findrow("$Comp",0);
    while(-1!=row){
        row = findrow("\""+fieldentry+"\"", 2, row+1);
        if(Tableread(row,10)=="\""+fieldname+"\"") break;
    }
    return row;
}

int Table::findKiCadSCHRef(string fieldentry)
{
    int row;
    row = findrow("$Comp",0);
    while(-1!=row){
        row = findrow("\""+fieldentry+"\"", 2, row+1);
        if((Tableread(row,1)=="0")&&Tableread(row,0)=="F") break;
    }
    return row;
}

int Table::findKiCadSCHVal(string fieldentry)
{
    int row;
    row = findrow("$Comp",0);
    while(-1!=row){
        row = findrow("\""+fieldentry+"\"", 2, row+1);
        if((Tableread(row,1)=="1")&&Tableread(row,0)=="F") break;
    }
    return row;
}

int Table::findKiCadSCHFP(string fieldentry)
{
    int row;
    row = findrow("$Comp",0);
    while(-1!=row){
        row = findrow("\""+fieldentry+"\"", 2, row+1);
        if((Tableread(row,1)=="2")&&Tableread(row,0)=="F") break;
    }
    return row;
}

int Table::findKiCadSCHDS(string fieldentry)
{
    int row;
    row = findrow("$Comp",0);
    while(-1!=row){
        row = findrow("\""+fieldentry+"\"", 2, row+1);
        if((Tableread(row,1)=="3")&&Tableread(row,0)=="F") break;
    }
    return row;
}

int Table::getCompbeginrow(int row)
{
    row = findrow_bw("$Comp",0,row);
    return row;
}

int Table::getCompendrow(int row)
{
    row = findrow("$EndComp",0,row);
    return row;
}

string Table::getEntry(int row, string fieldname)
{
    int startrow, endrow;
    startrow = getCompbeginrow(row);
    endrow = getCompendrow(row);
    row = findrow("\""+fieldname+"\"",10,startrow);
    if(row>endrow) return "";
    return Tableread(row,2);
}

string Table::getRef(int row)
{
    int startrow, endrow;
    startrow = getCompbeginrow(row);
    endrow = getCompendrow(row);
    row = startrow;
    while(Tableread(row,0)!="F"){
        row = findrow("0",1,row+1);
        if(-1==row) return "";
    }
    if(row>endrow) return "";
    return Tableread(row,2);
}

string Table::getVal(int row)
{
    int startrow, endrow;
    startrow = getCompbeginrow(row);
    endrow = getCompendrow(row);
    row = startrow;
    while(Tableread(row,0)!="F"){
        row = findrow("1",1,row+1);
        if(-1==row) return "";
    }
    if(row>endrow) return "";
    return Tableread(row,2);
}

string Table::getFP(int row)
{
    int startrow, endrow;
    startrow = getCompbeginrow(row);
    endrow = getCompendrow(row);
    row = startrow;
    while(Tableread(row,0)!="F"){
        row = findrow("2",1,row+1);
        if(-1==row) return "";
    }
    if(row>endrow) return "";
    return Tableread(row,2);
}

string Table::getDS(int row)
{
    int startrow, endrow;
    startrow = getCompbeginrow(row);
    endrow = getCompendrow(row);
    row = startrow;
    while(Tableread(row,0)!="F"){
        row = findrow("3",1,row+1);
        if(-1==row) return "";
    }
    if(row>endrow) return "";
    return Tableread(row,2);
}
