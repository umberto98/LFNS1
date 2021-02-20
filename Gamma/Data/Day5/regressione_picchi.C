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
using namespace TMath;

//FIXME Aggiungere dati a metaà grafico (canale 500)
void regressione_picchi()
{
    int n = 3;

    float X[] = {
        1449, 1665, 1762
    };

    float sX[] = {
      12, 10, 10
    };

    float Y[] = {
      4.78, 5.48, 5.80
    };

    float sY[]{
      1e-7, 1e-7, 1e-7
    };

    float sY0[]{
       0,0,0
    };

    
    TCanvas *c1 = new TCanvas("c1","Grafico E(CHN)",100,50,800,600);
    c1->SetFillColor(0);
    c1->cd();
    // Istanzio il grafico
    TGraphErrors *graph = new TGraphErrors(n, X, Y, sX, sY);
    graph->SetMarkerSize(0);
    graph->SetMarkerStyle(1);
    // Titolo del grafico
    graph->SetTitle("E(CHN)");
    // Titoli degli assi
    graph->GetXaxis()->SetTitle("CHN");
    graph->GetYaxis()->SetTitle("E (MeV)");
    gPad->SetLeftMargin(0.15);
    // graph->SetMinimum(0);
    graph->GetXaxis()->SetLimits(0, 2048);
    graph->GetYaxis()->SetLimits(0.,6.);
    graph->GetYaxis()->SetTitleOffset(2.);
    graph->SetMarkerStyle(3);
    graph->SetMarkerSize(1);
    graph->SetMarkerColor(2);
    graph->Draw("AP");

    double rad = 5.08/2.;
    double dist = 9.3;
    TF1 *funz1 = new TF1("funz1","2*[0]*[0]*TMath::ACos([1]*TMath::Sin(TMath::DegToRad(x))/(2*rad))-0.5*[1]*TMath::Sin(TMath::DegToRad(x))*TMath::Sqrt(4.*[0]*[0]-[1]*TMath::Sin(TMath::DegToRad(x))*TMath::Sin(TMath::DegToRad(x))*[1])",-35,35,2);
    funz1->SetParameter(0, 5.08/2.);
   
    funz1->SetParameter(1, 9.3);    
//funz1->SetParNames("k", "p");
    cout << "\n\n --- Ipotesi:  " << funz1->GetTitle() << "  ---\n" << endl;
    funz1->SetLineColor(4);
    //graph->Fit("funz1","RM+");
    funz1->Draw();
    //cout << "Chi^2:" << funz1->GetChisquare() << ", number of DoF: " << funz1->GetNDF() << " (Probability: " << funz1->GetProb() << ")." << endl;

    cout << "---------------------------------------------------------------------" << endl;

    gStyle->SetOptFit();

}
