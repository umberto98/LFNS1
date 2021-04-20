#include "Riostream.h"
#include "TH1D.h"
#include "TFile.h"

const string strin = "<<ROI>>"; //skip dall'inizio a <<ROI>>
const string strfin = "<<DATA>>"; //e da <<ROI>> fino a <<DATA>>
const int ch = 2048; //numero canali MCA 

bool checkstr(string a, string b); //fa un check su due stringhe

void ReadMCA () {

    TH1D *htriple = new TH1D("htriple"," Sorgente Tripla - Americio/Curio/Neptunio ",ch,0.,(double)ch);

    ifstream input("./Data/Sorgente Tripla - Calibrazione/tre-picchi-2.mca");

    if (!input) {
        cout<<" FILE NON ESISTENTE \n";
        return;
    }

    string line;
    bool roi = false;
    bool data = false;
    double fillh;    
    int cntbin = 1;

    //----------------
    //LETTURA FILE MCA        
    //----------------
        
    while(getline(input,line)) {

        if (checkstr(strin,string(line))) roi=true;
        else if (checkstr(strfin,string(line))) {
            data=true;
            roi=true; //a volte non c'è la roi
        }
        else if (data && roi) break;
        else continue;

    }

    while(input>>fillh) htriple->SetBinContent(cntbin++,fillh); //legge i canali in ordine

    if (cntbin!=ch) { //check su numeri bin
        cout<<" IL VERO NUMERO DI BIN E' "<<cntbin<<endl;
        return;
    }

    TFile *hfile = new TFile("hcal.root","recreate");
    htriple->Write();
    
}

bool checkstr(string a, string b) {

    int sza = a.length();
    int szb = b.length();
    int minsz = sza;
    if (szb<sza) minsz = szb;

    for(int i=0;i<minsz;i++) {
        //cout<<" "<<a[i]<<" vs "<<b[i]<<endl;
        if (a[i] != b[i]) return false;
    }
    return true;
}