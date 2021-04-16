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

    TH1D* hpos[10]; //10 istogrammi a tensioni positive
    TH1D* hneg[10]; //10 istogrammi a tensioni negative

    char hposname[50]; //nome degli istogrammi positivi
    char hnegname[50]; //nome degli istogrammi negativi
    char hpostitle[50]; //titolo degli istogrammi positivi
    char hnegtitle[50]; //titolo degli istogrammi negativi

    int cntpos = 0; //contatore da 0 a 9 per riempire l'array di istogrammi
    int cntneg = 0; //stesso ma per i negativi

    for(int i=50;i<=500;i=i+50) {

        sprintf(filepos,"./Data/Part2/s-%d.mca",i);
        sprintf(fileneg,"./Data/Part2/s-m%d.mca",i);
        ifstream inputpos(filepos);
        ifstream inputneg(fileneg);

        if (!inputpos) {
            cout<<" FILE "<<filepos<<" NON ESISTENTE \n";
            return;
        }
        if (!inputneg) {
            cout<<" FILE "<<fileneg<<" NON ESISTENTE \n";
            return;
        }

        sprintf(hposname,"hpos_%d",cntpos);
        sprintf(hnegname,"hneg_%d",cntneg);
        sprintf(hpostitle," SPETTRO MCA / HV = %d V",i);
        sprintf(hnegtitle," SPETTRO MCA / HV = - %d V",i);

        hpos[cntpos] = new TH1D(hposname,hpostitle,ch,0.,(double)ch);
        hneg[cntneg] = new TH1D(hnegname,hnegtitle,ch,0.,(double)ch);

        int cntbinhpos = 1; //parte da 1 e arriva a 1024 e serve a riempire i bin degli istogrammi in ordine
        int cntbinhneg = 1; //stesso ma per i negativi

        int fillhpos; //serve per riempire l'istogramma e leggere il file
        int fillhneg;

        string line;
        bool roi = false;
        bool data = false;

        //------------------------
        //LETTURA FILE HV POSITIVI
        //------------------------

        cout<<" Leggendo il file "<<filepos<<endl; 
        
        while(getline(inputpos,line)) {

            if (checkstr(strin,string(line))) roi=true;
            else if (checkstr(strfin,string(line))) data=true;
            else if (data && roi) break;
            else continue;

        }

        while(inputpos>>fillhpos) hpos[cntpos]->SetBinContent(cntbinhpos++,fillhpos); //legge i canali

        //------------------------
        //LETTURA FILE HV NEGATIVI
        //------------------------

        roi = false;
        data = false;

        cout<<" Leggendo il file "<<fileneg<<endl; 
        
        while(getline(inputneg,line)) {

            if (checkstr(strin,string(line))) roi=true;
            else if (checkstr(strfin,string(line))) data=true;
            else if (data && roi) break;
            else continue;

        }

        while(inputneg>>fillhneg) hneg[cntneg]->SetBinContent(cntbinhneg++,fillhneg); //legge i canali

        if (cntbinhpos!=ch) {
            cout<<" IL VERO NUMERO DI BIN POS E' "<<cntbinhpos<<endl;
            return;
        }

        if (cntbinhneg!=ch) {
            cout<<" IL VERO NUMERO DI BIN NEG E' "<<cntbinhneg<<endl;
            return;
        }

        cntpos++;
        cntneg++;

    }

    TFile *hfile = new TFile("hmca.root","recreate");

    for(int i=0;i<10;i++) {
        hpos[i]->Write();
        hneg[i]->Write();
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