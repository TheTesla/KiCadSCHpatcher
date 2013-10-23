
PinDef* readPinlist(ifstream &pinFile)
{
    PinDef Pin[MAX_PINS];
    string line;
    string remainingstr;
    string table[3];
    signed pos, endpos, lastpos;
    int i;
    int cols[3];
    int col, row;
    int NUMcol, NAMEcol, TEXTcol; // Spaltennummer in der Tabelle

    if(pinFile.is_open()){
        // bis ==== Pinlist ==== vorspulen
        while(pinFile.good()){
            getline(pinFile, line);
            cout << line << endl;
            if(-1 != line.find("==== Pinlist ====")) break;
        }
        // bis Tabellenkopf vorspulen
        while(pinFile.good()){
            getline(pinFile, line);
            if(-1 != line.find("NAME")) break;
        }

        // Spaltenzuordnung finden - können beliebig angeordnet werden
        cols[0] = line.find("NUM");
        cols[1] = line.find("NAME");
        cols[2] = line.find("TEXT");

        col = 0;
        for(i=0;i<3;i++){
            if(cols[i] < cols[0]) col++;
        }
        NUMcol = col;
        col = 0;
        for(i=0;i<3;i++){
            if(cols[i] < cols[1]) col++;
        }
        NAMEcol = col;
        col = 0;
        for(i=0;i<3;i++){
            if(cols[i] < cols[2]) col++;
        }
        TEXTcol = col;

        cout << "NUMcol= " << NUMcol << " NAMEcol= " << NAMEcol << " TEXTcol= " << TEXTcol << endl;

        row = 0;
        while(pinFile.good()){
            getline(pinFile, line);
            if(-1 == line.find(",")) continue; //keine gültige Zeile/Leerzeile
            if(-1 != line.find("---- END ----")) return Pin;
            remainingstr = line;

            for(i=0;i<3;i++){
                pos = remainingstr.find_first_not_of(" ");
                if(pos < 0) pos = 0;
                remainingstr = remainingstr.substr(pos);
                endpos = remainingstr.find_first_of(",");
                if(-1==endpos){// letzter Eintrag ohne Komma am Ende
                    table[2] = remainingstr;
                }
                else{
                    table[i] = remainingstr.substr(0,endpos);
                    remainingstr = remainingstr.substr(endpos+1);
                }
                lastpos = table[i].find_last_not_of(" ");
                if(lastpos>-1) table[i] = table[i].substr(0,lastpos+1);
            }
            cout << table[0] << " " << table[1] << " " << table[2] << " " << endl;
            stringstream(table[NUMcol]) >> Pin[row].num;
            Pin[row].name = table[NAMEcol];
            Pin[row].text = table[TEXTcol];
            row++;
        }
    }
    return Pin;
}
