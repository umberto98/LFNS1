#include "Riostream.h"
#include "TH1D.h"
#include "TFile.h"

const string strin = "<<ROI>>"; //skip dall'inizio a <<ROI>>
const string strfin = "<<DATA>>"; //e da <<ROI>> fino a <<DATA>>
const int ch = 1024; //numero canali MCA 

bool checkstr(string a, string b); //fa un check su due stringhe

void ReadMCA () {

    char filepos[50]; //nome del file di quelli a tensioni positive

    TH1D* hpos[6]; //10 istogrammi a tensioni positive

    char hposname[50]; //nome degli istogrammi positivi
    char hpostitle[50]; //titolo degli istogrammi positivi

    int cntpos = 0; //contatore da 0 a 9 per riempire l'array di istogrammi

    for(int i=0;i<=10;i=i+2) {

        sprintf(filepos,"CalibrazTempi_%d.mca",i);
        ifstream inputpos(filepos);

        if (!inputpos) {
            cout<<" FILE "<<filepos<<" NON ESISTENTE \n";
            return;
        }


        sprintf(hposname,"hpos_%d",cntpos);
        sprintf(hpostitle," SPETTRO TAC / Delay = %d ns",i);

        hpos[cntpos] = new TH1D(hposname,hpostitle,ch,0.,(double)ch);

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

            if (checkstr(strfin,string(line))) data=true;
            else if (data) break;
            else continue;

        }

        while(inputpos>>fillhpos) {
  	 hpos[cntpos]->SetBinContent(cntbinhpos++,fillhpos); //legge i canali
        }

       

        cntpos++;

    }

    TFile *hfile = new TFile("hmca.root","recreate");

    for(int i=0;i<6;i++) {
        hpos[i]->Write();
 
    }
    hfile->Close();
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