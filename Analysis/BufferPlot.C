#include "Riostream.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TFile.h"
#include "TStyle.h"

//SENSIBILITA
const double adcsens = 0.25; //-0.25 pC per canale -> prendiamo in modulo
const double tdcsens = 2.5; //2.5 ns per canale
const double clksens = 100; //100 ns per canale

//DYNAMIC RANGE
const int adcdyn = 2048; //max num canali adc
const int tdcdyn = 2048; //max num canali tdc
const int clkdyn = 100; //sappiamo solo il full scale range 

//FULL SCALE RANGE
const double adcrng = adcsens*adcdyn;
const double tdcrng = tdcsens*tdcdyn; 
const double clkrng = clksens*clkdyn; //10 microsec

//ERROR MASK
long int check1 = 1111111100000000; //7 canali accesi
long int check2 = 1111111110000000; //8 canali accesi solo per i primi eventi -> c'è un modulo che viene spento 

//CONVERSIONI
double clkscal (int ch) {return clksens*ch;} 
double tdc (int ch) {return tdcsens*ch;}
double adc (int ch) {return adcsens*ch;}

//MAIN
void BufferPlot () {

    TFile *wfile = new TFile("hplot.root","recreate");

    int ev; //num evento
    long int mask; //error mask
    int clk, scal, inhscal, tdcch, adc10, adc11, pu; //clk scaler, scaler, scaler inibito, tdc, adc ch10, adc ch11 e pattern unit

    TH1D *hadc10 = new TH1D("hadc10"," ADC CH10 - S1 ",adcdyn,0.,adcrng);//s1
    TH1D *hadc11 = new TH1D("hadc11"," ADC CH11 - SG ",adcdyn,0.,adcrng);//sg
    TH1D *hadc11flag = new TH1D("hadc11flag"," ADC CH11 - SG - No Electrons ",adcdyn,0.,adcrng);//sg  
    TH1D *hclk = new TH1D("hclk"," CLK SCALER ",clkdyn,0.,clkrng);//clk scaler
    TH1D *htdc = new TH1D("htdc"," TDC ",tdcdyn,0.,tdcrng);//tdc

    hadc10->GetXaxis()->SetTitle(" Charge [pC] ");
    hadc10->GetYaxis()->SetTitle(" Entries ");
    hadc11->GetXaxis()->SetTitle(" Charge [pC] ");
    hadc11->GetYaxis()->SetTitle(" Entries ");
    hadc11flag->GetXaxis()->SetTitle(" Charge [pC] ");
    hadc11flag->GetYaxis()->SetTitle(" Entries ");
    hclk->GetXaxis()->SetTitle(" Time [ns] ");
    hclk->GetYaxis()->SetTitle(" Entries ");
    htdc->GetXaxis()->SetTitle(" Time [ns] ");
    htdc->GetYaxis()->SetTitle(" Entries ");

    ifstream buff("piedestallo_05_6us_final.txt");

    if (!buff) {
        cout<<"Attenzione: il file dati non esiste \n";
        return;
    }
    
    string line;
    getline(buff,line); //salta la prima linea

    while (buff>>ev>>mask>>clk>>scal>>inhscal>>tdcch>>adc10>>adc11>>pu) {
        if (ev%10000==0) cout<<" Leggendo e processando l'evento "<<ev<<endl; 
        if (mask==check1 || mask==check2) {
            hadc10->Fill(adc(adc10));
            if (pu==0) hadc11->Fill(adc(adc11));
            else hadc11flag->Fill(adc(adc11));
            hclk->Fill(clkscal(clk));
            htdc->Fill(tdc(tdcch));
        }
    }

    hadc10->Rebin(8);
    hadc11->Rebin(8);
    hadc11flag->Rebin(8);
    htdc->Rebin(8);

    gStyle->SetOptStat(0);

    TCanvas *c1 = new TCanvas();
    hadc10->Draw("histo");
    c1->SaveAs("s1adc.png");

    TCanvas *c2 = new TCanvas();
    hadc11->Draw("histo");
    c2->SaveAs("sgadc.png");

    TCanvas *c3 = new TCanvas();
    hadc11flag->Draw("histo");
    c3->SaveAs("sgadc.png");

    TCanvas *c4 = new TCanvas();
    hclk->Draw("histo");    
    c4->SetLogy(1);
    c4->SaveAs("clkscal.png");

    TCanvas *c5 = new TCanvas();
    htdc->Draw("histo");
    c5->SetLogy(1);
    c5->SaveAs("tdc.png");

    TCanvas *c6 = new TCanvas();
    hadc11->Draw("histo");
    hadc11flag->SetLineColor(kRed);
    hadc11flag->Draw("hist same");
    c6->BuildLegend();
    c6->SaveAs("sgboth.png");

    wfile->Write();




}