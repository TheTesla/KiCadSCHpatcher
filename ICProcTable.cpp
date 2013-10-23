
#include "ICProcTable.h"


ICdevSym readProperties(Table tab)
{
    int startrow, endrow, row;
    ICdevSym Sym;
    
    startrow = tab.findrow("==== Properties ====");
    endrow = tab.findrow("---- END ----",0,startrow);
    
    row = tab.findrow("NAME",0,startrow);
    if(row < endrow && row != -1) Sym.name = tab.Tableread(row,1);
    row = tab.findrow("REFERENCE",0,startrow);
    if(row < endrow && row != -1) Sym.reference = tab.Tableread(row,1);
    row = tab.findrow("PACKAGE",0,startrow);
    if(row < endrow && row != -1) Sym.Package = tab.Tableread(row,1);
    row = tab.findrow("LIBRARY",0,startrow);
    if(row < endrow && row != -1) Sym.Library = tab.Tableread(row,1);
    row = tab.findrow("MINWIDTH",0,startrow);
    if(row < endrow && row != -1) stringstream(tab.Tableread(row,1)) >> Sym.width;
    row = tab.findrow("MINHEIGHT",0,startrow);
    if(row < endrow && row != -1) stringstream(tab.Tableread(row,1)) >> Sym.height;

    return Sym; 
}

ICOptions readOptions(Table tab)
{
    int startrow, endrow, row;
    ICOptions Opt;
    Opt.stepsize = 1;
    
    startrow = tab.findrow("==== Options ====");
    endrow = tab.findrow("---- END ----",0,startrow);
    
    row = tab.findrow("STEPSIZE",0,startrow);
    if(row < endrow && row != -1) stringstream(tab.Tableread(row,1)) >> Opt.stepsize;

    return Opt; 
}

All4PinDescrVec readPinDescr(Table tab)
{
    All4PinDescrVec AllPD;
    int startPinlist, endPinlist;
    int startArrangement, endArrangement;
    int rowArr, rowPL;
    int NAMEcolArr, ABSSPACEcolArr, RELSPACEcolArr;
    int NAMEcolPL, NUMcolPL, TEXTcolPL, ELECTRICAL_TYPEcolPL, PIN_TYPEcolPL;
    int num;
    unsigned long relspace, absspace;
    string name, text;
    string com, arg, dummy;
    char side;
    string electrical_type;
    string pin_type;

    
    AllPD.numbPinsL = 0;
    AllPD.numbPinsR = 0;
    AllPD.numbPinsU = 0;
    AllPD.numbPinsD = 0;
    
    startPinlist = tab.findrow("==== Pinlist ====");
    endPinlist = tab.findrow("---- END ----", 0, startPinlist);
    startArrangement = tab.findrow("==== Arrangement ====");
    endArrangement = tab.findrow("---- END ----", 0, startArrangement);
    
    // Spaltenzuordnung in Pinlist finden
    NAMEcolPL = tab.findcol("NAME",startPinlist+1);
    NUMcolPL  = tab.findcol("NUM",startPinlist+1);
    TEXTcolPL  = tab.findcol("TEXT",startPinlist+1);
    ELECTRICAL_TYPEcolPL = tab.findcol("ELECTRICAL_TYPE",startPinlist+1);
    PIN_TYPEcolPL = tab.findcol("PIN_TYPE",startPinlist+1);
    
    // Spaltenzuordnung in Arrangement finden
    NAMEcolArr = tab.findcol("NAME",startArrangement+1);
    ABSSPACEcolArr = tab.findcol("ABSSPACE",startArrangement+1);
    RELSPACEcolArr = tab.findcol("RELSPACE",startArrangement+1);
    
    // Arrangement in gleicher Reihenfolge wie am Symbol angeordnet
    // deshalb Arrangement der Reihe nach durchgehen
    // erster Durchlauf - Speicherbedarf ermitteln
    side = 0;
    for(rowArr=startArrangement+2;rowArr<endArrangement;rowArr++){
        com = tab.Tableread(rowArr, 0);
        arg = tab.Tableread(rowArr, 1);
        dummy = tab.Tableread(rowArr, 2);
        if(0 == com.compare("SIDE") && 0 == dummy.compare("")) side = arg[0];
        else{
            name = tab.Tableread(rowArr, NAMEcolArr);
            rowPL = tab.findrow(name, NAMEcolPL, startPinlist);
            if(rowPL > endPinlist || rowPL == -1){
                cout << "Kein Eintrag mit NAME \"" << name << "\" in Pinlist." << endl; 
            }
            else{
                switch(side){
                    case 'L':
                        AllPD.numbPinsL++;
                        break;
                    case 'R':
                        AllPD.numbPinsR++;
                        break;
                    case 'U':
                        AllPD.numbPinsU++;
                        break;
                    case 'D':
                        AllPD.numbPinsD++;
                        break;
                }
            }
        }
    }
    AllPD.PDVec_L = new PinDescr[AllPD.numbPinsL];
    AllPD.PDVec_R = new PinDescr[AllPD.numbPinsR];
    AllPD.PDVec_U = new PinDescr[AllPD.numbPinsU];
    AllPD.PDVec_D = new PinDescr[AllPD.numbPinsD];

    AllPD.numbPinsL = 0;
    AllPD.numbPinsR = 0;
    AllPD.numbPinsU = 0;
    AllPD.numbPinsD = 0;
    
    // zweiter Durchlauf - alle 4 PinDescr-Vektoren füllen
    side = 0;
    for(rowArr=startArrangement+2;rowArr<endArrangement;rowArr++){
        com = tab.Tableread(rowArr, 0);
        arg = tab.Tableread(rowArr, 1);
        dummy = tab.Tableread(rowArr, 2);
        if(0 == com.compare("SIDE") && 0 == dummy.compare("")) side = arg[0];
        else{
            name = tab.Tableread(rowArr, NAMEcolArr);
            rowPL = tab.findrow(name, NAMEcolPL, startPinlist);
            if(rowPL > endPinlist || rowPL == -1){
                cout << "Kein Eintrag mit NAME \"" << name << "\" in Pinlist." << endl; 
            }
            else{
                stringstream(tab.Tableread(rowPL, NUMcolPL)) >> num;
                text = tab.Tableread(rowPL, TEXTcolPL);
                stringstream(tab.Tableread(rowArr, RELSPACEcolArr)) >> relspace;
                stringstream(tab.Tableread(rowArr, ABSSPACEcolArr)) >> absspace;
                electrical_type = tab.Tableread(rowPL, ELECTRICAL_TYPEcolPL);
                pin_type = tab.Tableread(rowPL, PIN_TYPEcolPL);
                switch(side){
                    case 'L':
                        AllPD.PDVec_L[AllPD.numbPinsL].name = text;
                        AllPD.PDVec_L[AllPD.numbPinsL].num = num;
                        AllPD.PDVec_L[AllPD.numbPinsL].RelSpace = relspace;
                        AllPD.PDVec_L[AllPD.numbPinsL].AbsSpace = absspace;
                        AllPD.PDVec_L[AllPD.numbPinsL].electrical_type = electrical_type;
                        AllPD.PDVec_L[AllPD.numbPinsL].pin_type = pin_type;
                        AllPD.numbPinsL++;
                        break;
                    case 'R':
                        AllPD.PDVec_R[AllPD.numbPinsR].name = text;
                        AllPD.PDVec_R[AllPD.numbPinsR].num = num;
                        AllPD.PDVec_R[AllPD.numbPinsR].RelSpace = relspace;
                        AllPD.PDVec_R[AllPD.numbPinsR].AbsSpace = absspace;
                        AllPD.PDVec_R[AllPD.numbPinsR].electrical_type = electrical_type;
                        AllPD.PDVec_R[AllPD.numbPinsR].pin_type = pin_type;
                        AllPD.numbPinsR++;
                        break;
                    case 'U':
                        AllPD.PDVec_U[AllPD.numbPinsU].name = text;
                        AllPD.PDVec_U[AllPD.numbPinsU].num = num;
                        AllPD.PDVec_U[AllPD.numbPinsU].RelSpace = relspace;
                        AllPD.PDVec_U[AllPD.numbPinsU].AbsSpace = absspace;
                        AllPD.PDVec_U[AllPD.numbPinsU].electrical_type = electrical_type;
                        AllPD.PDVec_U[AllPD.numbPinsU].pin_type = pin_type;
                        AllPD.numbPinsU++;
                        break;
                    case 'D':
                        AllPD.PDVec_D[AllPD.numbPinsD].name = text;
                        AllPD.PDVec_D[AllPD.numbPinsD].num = num;
                        AllPD.PDVec_D[AllPD.numbPinsD].RelSpace = relspace;
                        AllPD.PDVec_D[AllPD.numbPinsD].AbsSpace = absspace;
                        AllPD.PDVec_D[AllPD.numbPinsD].electrical_type = electrical_type;
                        AllPD.PDVec_D[AllPD.numbPinsD].pin_type = pin_type;
                        AllPD.numbPinsD++;
                        break;
                }
            }
        }
    }    
    
    return AllPD;
}


