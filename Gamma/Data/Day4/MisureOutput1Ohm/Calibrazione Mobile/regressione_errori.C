#include <iostream>           // serve a stampare a video il testo (uso di cout e endl, per esempio)
#include <TGraphErrors.h>
#include <TAxis.h>
#include <TCanvas.h>
#include <TF1.h>
#include <iomanip>            // serve a manipolare l'output a video
#include <TLegend.h>
#include <TObject.h>
#include <TLatex.h>
// #include <TPaveStats.h>
#include <TMath.h>

using namespace std;

const float k = 1.413;
const float p = -21.16;
const float kerr = 0.4233;
const float perr = 30.42; 

//FIXME Aggiungere dati a metaà grafico (canale 500)
void regressione_errori()
{
    int n = 4;

	
    float E[] = {
	511, 1275, 1173.2, 1332.5
    };

    float sE[] = {
        0,0,0,0,0
    };

    float dCHN[] = {
	1.18654e+01, 1.97727e+01, 1.81102e+01, 2.02414e+01
    };

    float dCHNerr[] = {
    	2.60729e-02, 1.06612e-01, 2.54988e-01, 3.67709e-01
    };
    
    float Y[4];
    for (int i =0; i<4; i++){
    	Y[i] = dCHN[i]*k/E[i];
    }

    float sY[4];
    for(int i=0; i<4; i++){
    	sY[i] = TMath::Sqrt(dCHN[i]*dCHN[i]*kerr*kerr+k*k*dCHNerr[i]*dCHNerr[i])/E[i];
    }


    TCanvas *c1 = new TCanvas("c1","Grafico E(CHN)",100,50,800,600);
    c1->SetFillColor(0);
    c1->cd();
    // Istanzio il grafico
    TGraphErrors *graph = new TGraphErrors(n, E, Y, sE, sY);
    graph->SetMarkerSize(0);
    graph->SetMarkerStyle(1);
    // Titolo del grafico
    graph->SetTitle("R(E)");
    // Titoli degli assi
    graph->GetXaxis()->SetTitle("E");
    graph->GetYaxis()->SetTitle("R");
    gPad->SetLeftMargin(0.15);
    // graph->SetMinimum(0);
    graph->GetXaxis()->SetLimits(0, 1400);
    graph->GetYaxis()->SetTitleOffset(2.);
    graph->SetMarkerStyle(3);
    graph->SetMarkerSize(1);
    graph->SetMarkerColor(2);
    graph->Draw("AP");


    TF1 *funz1 = new TF1("funz1","[0]/(x)^0.5 + [1]", 0, 1400);
    funz1->SetParNames("k", "p");
    cout << "\n\n --- Ipotesi:  " << funz1->GetTitle() << "  ---\n" << endl;
    funz1->SetLineColor(4);
    graph->Fit("funz1","RM+");

    cout << "Chi^2:" << funz1->GetChisquare() << ", number of DoF: " << funz1->GetNDF() << " (Probability: " << funz1->GetProb() << ")." << endl;

    cout << "---------------------------------------------------------------------" << endl;

    gStyle->SetOptFit();
/*
    TF1 *funz2 = new TF1("funz2","[0]*x*x", 0, 900);
    cout << "\n\n --- Ipotesi:  " << funz2->GetTitle() << "  ---\n" << endl;
    funz1->SetLineColor(5);
    graph->Fit("funz2","RM+");

    cout << "Chi^2:" << funz2->GetChisquare() << ", number of DoF: " << funz2->GetNDF() << " (Probability: " << funz2->GetProb() << ")." << endl;

    cout << "---------------------------------------------------------------------" << endl;
*/
}
