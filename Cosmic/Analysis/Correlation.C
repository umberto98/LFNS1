#include "TTree.h"
#include "TBranch.h"
#include "Riostream.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TPad.h"

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

//OVERFLOW ADC -> Sono gli ultimi 24 canali 
const int adcov10 = 24; //tolgo gli ultimi 24 canali di overflow 
const int adcov11 = 24; //stessa cosa, ma al canale 11

//OVERFLOW CLK SCALER -> Canale dal 63 compreso in poi
const int clkov = 63;

//CORRELAZIONI
bool corr = true; //true se si voglioni i plot di correlazione

void Correlation () {
    
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
        if (ev%100000==0) cout<<" Leggendo e processando l'evento "<<ev<<endl; 
        /*if (event.adc10<ped10 || event.adc11<ped11) {
            cout<<" Canali NEGATIVI "<<ev<<endl;
        }*/
        if (event.pu==0 /*&& event.adc10<(adcdyn-adcov10) && event.adc11<(adcdyn-adcov11) && event.clk<62*/) { //cuts
            hadc10->Fill(adc(event.adc10-ped10)); //togliamo i canali del piedistallo
            hadc11->Fill(adc(event.adc11-ped11));
            hclk->Fill(clkscal(event.clk));
            htdc->Fill(tdcconv(event.tdc));
            hcorr11->Fill(adc(event.adc11),tdcconv(event.tdc));
            hcorr10->Fill(adc(event.adc10),tdcconv(event.tdc));
            hcorr11clk->Fill(adc(event.adc11),clkscal(event.clk));
            hcorr10clk->Fill(adc(event.adc10),clkscal(event.clk));
        }
        if (event.pu==1 && event.adc10<(adcdyn-adcov10) && event.adc11<(adcdyn-adcov11)) hadc11flag->Fill(adc(event.adc11-ped11));
    }

    hadc10->Rebin(8);
    hadc11->Rebin(8);
    hadc11flag->Rebin(8);
    htdc->Rebin(8);

    hcorr11->Rebin2D(8);
    hcorr10->Rebin2D(8);
    hcorr11clk->RebinX(8);
    hcorr10clk->RebinX(8);

    gStyle->SetOptStat(0);

    if (corr==true) {

        TCanvas *c1 = new TCanvas("c1","c1",1920,1080);
        c1->Divide(2,2);
        c1->cd(2); //in alto a dx
        hcorr11->DrawCopy("COLZ");
        gPad->SaveAs("hcorr11_1.png");
        c1->cd(4); //in basso a dx
        hadc11->Draw("hist");
        gPad->SaveAs("hcorr11_2.png");
        c1->cd(1); //in alto a sx
        gPad->SetLogy(1);
        htdc->Draw("hist");
        gPad->SaveAs("hcorr11_3.png");
        c1->SaveAs("hcorr11.png");

        TCanvas *c2 = new TCanvas("c2","c2",1920,1080);
        c2->Divide(2,2);
        c2->cd(2); //in alto a dx
        hcorr10->DrawCopy("COLZ");
        gPad->SaveAs("hcorr10_1.png");
        c2->cd(4); //in basso a dx
        hadc10->Draw("hist");
        gPad->SaveAs("hcorr10_2.png");
        c2->cd(1); //in alto a sx
        gPad->SetLogy(1);
        htdc->Draw("hist");
        gPad->SaveAs("hcorr10_3.png");
        c2->SaveAs("hcorr10.png");

        TCanvas *c3 = new TCanvas("c3","c3",1920,1080);
        c3->Divide(2,2);
        c3->cd(2); //in alto a dx
        hcorr11clk->DrawCopy("COLZ");
        gPad->SaveAs("hcorr11clk_1.png");
        c3->cd(4); //in basso a dx
        hadc11->Draw("hist");
        gPad->SaveAs("hcorr11clk_2.png");
        c3->cd(1); //in alto a sx
        gPad->SetLogy(1);
        hclk->Draw("hist");
        gPad->SaveAs("hcorr11clk_3.png");
        c3->SaveAs("hcorr11clk.png");

        TCanvas *c4 = new TCanvas("c4","c4",1920,1080);
        c4->Divide(2,2);
        c4->cd(2); //in alto a dx
        hcorr10clk->DrawCopy("COLZ");
        gPad->SaveAs("hcorr10clk_1.png");
        c4->cd(4); //in basso a dx
        hadc10->Draw("hist");
        gPad->SaveAs("hcorr10clk_2.png");
        c4->cd(1); //in alto a sx
        gPad->SetLogy(1);
        hclk->Draw("hist");
        gPad->SaveAs("hcorr10clk_3.png");
        c4->SaveAs("hcorr10clk.png");

    }

    wfile->Write();



}