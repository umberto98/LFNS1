#include "Riostream.h"
#include "TH1D.h"
#include "TFile.h"

const string strin = "<<ROI>>"; //skip dall'inizio a <<ROI>>
const string strfin = "<<DATA>>"; //e da <<ROI>> fino a <<DATA>>
const int ch = 1024; //numero canali MCA 

bool checkstr(string a, string b); //fa un check su due stringhe

void ReadMCA () {

    char filepos[50]; //nome del file di quelli a tensioni positive
    char fileneg[50]; //nome del file di quelli a tensioni negative

    TH1D* hgamma[2]; //2 istogrammi dei gamma

    char hposname[50]; //nome degli istogrammi positivi
    char hpostitle[50]; //titolo degli istogrammi positivi

    int cntpos = 0; //contatore da 0 a 9 per riempire l'array di istogrammi

    for(int i=0;i<=1;i++) {

        if (i==0) sprintf(filepos,"SpettroCoConDelayFisso.mca");
	if (i==1) sprintf(filepos,"SpettroNaDelayFisso.mca");
        ifstream inputpos(filepos);

        if (!inputpos) {
            cout<<" FILE "<<filepos<<" NON ESISTENTE \n";
            return;
        }

        sprintf(hposname,"hday1_%d",cntpos);
        if (i==0) sprintf(hpostitle," SPETTRO MCA Co - Fisso");
	if (i==1) sprintf(hpostitle," SPETTRO MCA Na - Fisso");

        hgamma[cntpos] = new TH1D(hposname,hpostitle,ch,0.,(double)ch);

        int cntbinhpos = 1; //parte da 1 e arriva a 1024 e serve a riempire i bin degli istogrammi in ordine

        int fillhpos; //serve per riempire l'istogramma e leggere il file

        string line;
        bool roi = false;
        bool data = false;

        //------------------------
        //LETTURA FILE HV POSITIVI
        //------------------------

        cout<<" Leggendo il file "<<filepos<<endl; 
        
        while(getline(inputpos,line)) {

            //if (checkstr(strin,string(line))) roi=true;
            if (checkstr(strfin,string(line))) data=true;
            else if (data) break;
            else continue;

        }

        while(inputpos>>fillhpos) {
            hgamma[cntpos]->SetBinContent(cntbinhpos++,fillhpos); //legge i canali
        }

        if (cntbinhpos!=ch) {
            cout<<" IL VERO NUMERO DI BIN POS E' "<<cntbinhpos<<endl;
            return;
        }

        cntpos++;
    }

    TFile *hfile = new TFile("day2.root","recreate");

    for(int i=0;i<2;i++) {
        hgamma[i]->Write();
    }
    
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