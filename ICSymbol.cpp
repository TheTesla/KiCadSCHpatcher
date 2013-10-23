
#include "ICSymbol.h"

signed long intround(signed long x, unsigned long stepsize)
{
    signed long tmp;
    bool neg;
    neg = false;
    if(x < 0){
        neg = true;
        x = -x - 1;
    }
    tmp = x + stepsize/2;
    tmp/= stepsize;
    tmp*= stepsize;
    if(neg) return -tmp;
    return tmp;
}

void roundICcoords(ICdevSym &Sym, unsigned long PinEntryNum, unsigned long stepsize)
{
    int tmp,i;
    for(i=0;i<PinEntryNum;i++){
        Sym.Pins[i].posx = intround(Sym.Pins[i].posx, stepsize);
        Sym.Pins[i].posy = intround(Sym.Pins[i].posy, stepsize);
    }
    // Größe des Rechtecks runden
    Sym.width = intround(Sym.width, stepsize);        
    Sym.height = intround(Sym.height, stepsize);
    // Koordinaten des ersten Eckpunktes runden
    // -> Bauteilmitte ist dann nicht auf Koordinatenursprung,
    //    wenn Höhe oder Breite ungerade Schrittanzahl haben
    Sym.x1 = intround(Sym.x1, stepsize);
    Sym.y1 = intround(Sym.y1, stepsize);
}

void updateICSymSize(ICdevSym &Sym, PinDescr* PinList, unsigned long PinEntryNum, char PinDir)
{
    unsigned long i;
    unsigned long edge_length;

    edge_length = 0;

    //Pin-Liste durchgehen, Länge der Symbolkante bestimmen
    for(i=0;i<PinEntryNum;i++)
    {
        edge_length += PinList[i].AbsSpace;
    }
    
    //Symbolgröße anpassen
    if(PinDir == 'L' || PinDir == 'R'){
        if(Sym.height<edge_length) Sym.height = edge_length;
    }
    else{
        if(Sym.width<edge_length) Sym.width = edge_length;
    }
}

void ICSymPinPlace(ICdevSym &Sym, PinDescr* PinList, unsigned long PinEntryNum, char PinDir, unsigned long PinLength)
{
    unsigned long i;
    unsigned long rubberSpace; // verbleibender Raum, der relativ aufgeteilt wird
    unsigned long sumRelSpace; // Gesamtgröße der relativen Größenangaben
    float normrubSpace;
    unsigned long old_Pincount;
    signed long pos;
    
    sumRelSpace = 0;
    old_Pincount = Sym.Pincount;

    // erst gesamte Kantenlänge als relativ aufteilbarerer Raum
    if(PinDir=='L' || PinDir=='R'){
        rubberSpace = Sym.height;
    }
    else{
        rubberSpace = Sym.width;   
    }
    
    //Pin-Liste durchgehen, Länge der Symbolkante bestimmen, Eigenschaft in andere Struktur kopieren
    sumRelSpace += PinList[0].RelSpace;
    rubberSpace -= PinList[0].AbsSpace;
    for(i=1;i<PinEntryNum;i++)
    {
        sumRelSpace += PinList[i].RelSpace; // Summe der relativen Abstandsangaben
        rubberSpace -= PinList[i].AbsSpace; // feste Abstände können nicht mehr relativ aufgeteilt werden
        Sym.Pins[Sym.Pincount].name = PinList[i].name;
        Sym.Pins[Sym.Pincount].num = PinList[i].num;
        Sym.Pins[Sym.Pincount].electrical_type = PinList[i].electrical_type;
        Sym.Pins[Sym.Pincount].pin_type = PinList[i].pin_type;
        Sym.Pins[Sym.Pincount].direction = PinDir;
        Sym.Pins[Sym.Pincount].length = PinLength;
        Sym.Pincount++;
    }
    if(sumRelSpace){
        normrubSpace = float(rubberSpace)/float(sumRelSpace);   
    }
    else{
        normrubSpace = 0;
    }
    
    
    // Koordinaten des Rechteckes anpassen
    Sym.x1 = -signed(Sym.width/2);
    Sym.y1 = -signed(Sym.height/2);

    //Alle Pins an Symbol mit neuer Größe anordnen
    for(i=0;i<Sym.Pincount;i++){
        if(Sym.Pins[i].direction=='R') Sym.Pins[i].posx = -(Sym.x1 + Sym.width + Sym.Pins[i].length);
        if(Sym.Pins[i].direction=='L') Sym.Pins[i].posx = Sym.x1 + Sym.width + Sym.Pins[i].length;
        if(Sym.Pins[i].direction=='U') Sym.Pins[i].posy = -(Sym.y1 + Sym.height + Sym.Pins[i].length);
        if(Sym.Pins[i].direction=='D') Sym.Pins[i].posy = Sym.y1 + Sym.height + Sym.Pins[i].length;
    }
    
    //Neue Pins entlang der Symbolkante aufreihen
    if(PinDir=='L' || PinDir=='R'){
        pos = -signed(Sym.height/2);
        for(i=0;i<PinEntryNum;i++){
            pos += signed(PinList[i].AbsSpace) + float(PinList[i].RelSpace)*normrubSpace;
            Sym.Pins[i+old_Pincount].posy = -pos;
        }    
    }
    else{
        pos = -signed(Sym.width/2);
        for(i=0;i<PinEntryNum;i++){
            pos += signed(PinList[i].AbsSpace) + float(PinList[i].RelSpace)*normrubSpace;    
            Sym.Pins[i+old_Pincount].posx = pos;
        }
    }
} 

