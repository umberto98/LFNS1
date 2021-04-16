\\TDC Analysis

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

bool corr = false;

void TDC () {


    cout<<" //////////////////////////////////// \n CONFRONTO TDC - CLK SCALER CON E SENZA P.U. \n CALCOLO PESO NOISE E RESCALING \n /////////////////////////////////// \n";
    
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
    TH1D *hclk = new TH1D("hclk"," CLK - NO FLAG ",clkdyn,0.,clkrng);//clk scaler
    TH1D *htdc = new TH1D("htdc"," TDC ",tdcdyn,0.,tdcrng);//tdc
    TH1D *htdcflag = new TH1D("htdcflag"," TDC - FLAG ",tdcdyn,0,tdcrng); //tdc con flag
    TH1D *htdcnew = new TH1D("htdcnew"," TDC - NO NOISE ",tdcdyn,0.,tdcrng); //tdc senza noise
    TH1D *hclkflag = new TH1D("hclkflag"," CLK - FLAG ",clkdyn,0,clkrng); //clk con flag
    TH1D *hclknew = new TH1D("hclknew"," CLK - NO NOISE ",clkdyn,0.,clkrng); //tdc senza noise

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
        if (ev%100000==0) cout<<" Leggendo e processando l'evento "<<ev<<endl; 
        /*if (event.adc10<ped10 || event.adc11<ped11) {
            cout<<" Canali NEGATIVI "<<ev<<endl;
        }*/
        hadc10->Fill(adc(event.adc10-ped10)); //togliamo i canali del piedistallo
        if (event.pu==0) {
            hadc11->Fill(adc(event.adc11-ped11));
            hclk->Fill(clkscal(event.clk));
            htdc->Fill(tdcconv(event.tdc));
            hcorr11->Fill(adc(event.adc11),tdcconv(event.tdc));
            hcorr10->Fill(adc(event.adc10),tdcconv(event.tdc));
            hcorr11clk->Fill(adc(event.adc11),clkscal(event.clk));
            hcorr10clk->Fill(adc(event.adc10),clkscal(event.clk));
        }
        if (event.pu==1) {
            hadc11flag->Fill(adc(event.adc11-ped11));
            htdcflag->Fill(tdcconv(event.tdc));
            hclkflag->Fill(clkscal(event.clk));
        }
    }

    hadc10->Rebin(8);
    hadc11->Rebin(8);
    hadc11flag->Rebin(8);
    htdc->Rebin(8);
    htdcflag->Rebin(8);
    htdcnew->Rebin(8);

    TCanvas *c1 = new TCanvas();
    htdcflag->DrawCopy("hist");
    c1->SetLogy(1);
    c1->SaveAs("tdcflag.png");

    TCanvas *c2 = new TCanvas();
    hclkflag->DrawCopy("hist");
    c2->SetLogy(1);
    c2->SaveAs("clkflag.png");

    double fkstop = hclkflag->GetBinContent(63)+hclkflag->GetBinContent(64)+hclkflag->GetBinContent(65); //overflow clk scaler
    double noise = hclkflag->Integral()-fkstop; 
    double weight = fkstop/noise;

    cout<<" Peso Segnale "<<weight*100<<"% \n";
    cout<<" Peso Noise "<<(1-weight)*100<<"% \n";

    for (int i=63;i<=clkdyn+1;i++) {
        hclk->SetBinContent(i,0);
        hclkflag->SetBinContent(i,0);
    }

    //RE-SCALING CLK SCALER
    hclkflag->Scale((1-weight)/hclkflag->Integral());
    hclk->Scale(1./hclk->Integral());

    //RE-SCALING TDC
    htdcflag->Scale((1-weight)/htdcflag->Integral());
    htdc->Scale(1./htdc->Integral());

    cout<<" Check Integral CLK - NO FLAG "<<hclk->Integral()<<" e CLK - FLAG "<<hclkflag->Integral()<<endl;
    cout<<" Check Integral TDC - NO FLAG "<<htdc->Integral()<<" e TDC - FLAG "<<htdcflag->Integral()<<endl;

    //gStyle->SetOptStat(0);

    TCanvas *c12 = new TCanvas();
    hclk->SetLineColor(kRed);
    hclk->Draw("hist");
    hclkflag->Draw("hist same");
    hclknew->Add(hclk,hclkflag,1,-1);
    hclknew->SetLineColor(kGreen);
    //hclknew->Draw(" hist same");
    c12->SetLogy();
    c12->BuildLegend();
    c12->SaveAs("clknew.png");

    TCanvas *c13=new TCanvas();
    htdc->SetLineColor(kRed);
    htdc->Draw("hist");
    htdcflag->Draw("hist same");
    htdcnew->Add(htdc,htdcflag,1,-1);
    htdcnew->SetLineColor(kGreen);
    //htdcnew->Draw(" hist same");
    c13->SetLogy();
    c13->BuildLegend();
    c13->SaveAs("tdcnew.png");

    if (corr) {

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

    }

    wfile->Write();



}