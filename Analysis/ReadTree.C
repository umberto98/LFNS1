#include "TTree.h"
#include "TBranch.h"
#include "Riostream.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
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

//CONVERSIONI
double clkscal (int ch) {return clksens*ch;} 
double tdcconv (int ch) {return tdcsens*ch;}
double adc (int ch) {return adcsens*ch;}

//PIEDISTALLO
const int ped10 = 270; //channel
const int ped11 = 163; //channel

void ReadTree () {
    
    typedef struct {
        int clk, scal, inhscal, tdc, adc10, adc11, pu; //clk scaler, scaler, scaler inibito, tdc, adc ch10, adc ch11 e pattern unit
    } Ev;

    static Ev event;

    TFile tfile("MyTree.root"); //in lettura
    TTree *tree = (TTree*)tfile.Get("T");

    TFile *wfile = new TFile("hplot.root","recreate");
    TBranch *b1 = tree->GetBranch("Data");
    b1->SetAddress(&event.clk); 

    TH1D *hadc10 = new TH1D("hadc10"," ADC CH10 - S1 ",adcdyn,0.,adcrng);//s1
    TH1D *hadc11 = new TH1D("hadc11"," ADC CH11 - SG ",adcdyn,0.,adcrng);//sg
    TH1D *hadc11flag = new TH1D("hadc11flag"," ADC CH11 - SG - No Electrons ",adcdyn,0.,adcrng);//sg  
    TH1D *hclk = new TH1D("hclk"," CLK SCALER ",clkdyn,0.,clkrng);//clk scaler
    TH1D *htdc = new TH1D("htdc"," TDC ",tdcdyn,0.,tdcrng);//tdc

    TH2D *hcorr11 = new TH2D("hcorr11"," Correlation ADC (SG) - TDC ",adcdyn,0.,adcrng,tdcdyn,0.,tdcrng);
    TH2D *hcorr10 = new TH2D("hcorr10"," Correlation ADC (S1) - TDC ",adcdyn,0.,adcrng,tdcdyn,0.,tdcrng);
    TH2D *hcorr11clk = new TH2D("hcorr11clk"," Correlation ADC (SG) - TDC (CLK) ",adcdyn,0.,adcrng,clkdyn,0.,clkrng);
    TH2D *hcorr10clk = new TH2D("hcorr10clk"," Correlation ADC (S1) - TDC (CLK) ",adcdyn,0.,adcrng,clkdyn,0.,clkrng);

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

    hcorr11->GetXaxis()->SetTitle(" Charge [pC] ");
    hcorr11->GetYaxis()->SetTitle(" Time [ns] ");
    hcorr10->GetXaxis()->SetTitle(" Charge [pC] ");
    hcorr10->GetYaxis()->SetTitle(" Time [ns] ");
    hcorr11clk->GetXaxis()->SetTitle(" Charge [pC] ");
    hcorr11clk->GetYaxis()->SetTitle(" Time [ns] ");
    hcorr10clk->GetXaxis()->SetTitle(" Charge [pC] ");
    hcorr10clk->GetYaxis()->SetTitle(" Time [ns] ");

    for(int ev=0;ev<tree->GetEntriesFast();ev++) {
        tree->GetEvent(ev);
        if (ev%10000==0) cout<<" Leggendo e processando l'evento "<<ev<<endl; 
        /*if (event.adc10<ped10 || event.adc11<ped11) {
            cout<<" Canali NEGATIVI "<<ev<<endl;
        }*/
        hadc10->Fill(adc(event.adc10/*-ped10*/)); //togliamo i canali del piedistallo
        if (event.pu==0) {
            hadc11->Fill(adc(event.adc11/*-ped11*/));
            hclk->Fill(clkscal(event.clk));
            htdc->Fill(tdcconv(event.tdc));
            hcorr11->Fill(adc(event.adc11),tdcconv(event.tdc));
            hcorr10->Fill(adc(event.adc10),tdcconv(event.tdc));
            hcorr11clk->Fill(adc(event.adc11),clkscal(event.clk));
            hcorr10clk->Fill(adc(event.adc10),clkscal(event.clk));
        }
        else hadc11flag->Fill(adc(event.adc11/*-ped11*/));
    }

    hadc10->Rebin(8);
    hadc11->Rebin(8);
    hadc11flag->Rebin(8);
    htdc->Rebin(8);

    //gStyle->SetOptStat(0);

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

    TCanvas *c7 = new TCanvas();
    hcorr11->DrawCopy("COLZ");
    c7->SaveAs("hcorr11.png");

    TCanvas *c8 = new TCanvas();
    hcorr10->DrawCopy("COLZ");
    c8->SaveAs("hcorr10.png");

    TCanvas *c9 = new TCanvas();
    hcorr11clk->DrawCopy("COLZ");
    c9->SaveAs("hcorr11clk.png");

    TCanvas *c10 = new TCanvas();
    hcorr10clk->DrawCopy("COLZ");
    c10->SaveAs("hcorr10clk.png");

    wfile->Write();



}