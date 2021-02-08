#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TMath.h"
#include "TClonesArray.h"
#include "Riostream.h"
#include "TFile.h"

//ERROR MASK
long int check1 = 1111111100000000; //7 canali accesi
long int check2 = 1111111110000000; //8 canali accesi solo per i primi eventi -> c'è un modulo che viene spento 

//MAIN
void BufferPlot () {

   TFile *wfile = new TFile("MyTree.root","recreate");
   TTree *tree = new TTree("T","TTree con 2 branches");
   
   int ev;
   long int mask;
   typedef struct {
   int clk, scal, inhscal, tdc, adc10, adc11, pu;} Ev;
    
   static Ev event;
   //clk scaler, scaler, scaler inibito, tdc, adc ch10, adc ch11 e pattern unit
    tree->Branch("Data",&event.clk,"clk:scal:inhscal:tdc:adc10:adc11:pu/I");
    
    ifstream buff("data.txt");

    if (!buff) {
        cout<<"Attenzione: il file dati non esiste \n";
        return;
    }
    
    string line;
    getline(buff,line); //salta la prima linea

    while (buff>>ev>>mask>>event.clk>>event.scal>>event.inhscal>>event.tdc>>event.adc10>>event.adc11>>event.pu) {
        if (ev%10000==0) cout<<" Leggendo e processando l'evento "<<ev<<endl; 
        if (mask==check1 || mask==check2) 
           tree->Fill();
        else cout<< "Errore in evento: "<<ev<<endl;
       }
    

    wfile->Write();




}


