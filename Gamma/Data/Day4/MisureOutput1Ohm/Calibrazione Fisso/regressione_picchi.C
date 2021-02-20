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

using namespace std;

//FIXME Aggiungere dati a metaà grafico (canale 500)
void regressione_picchi()
{
    int n = 4;
//canali
    float X[] = {
        3.67253e+02, 8.80541e+02, 8.13172e+02, 9.17944e+02
    };

    float sX[] = {
      1.19599e+01, 1.93096e+01,  1.91448e+01, 1.96829e+01
    };

    float Y[] = {
      511, 1275, 1173.2, 1332.5
    };

    float sY[]{
      1e-7, 1e-7, 1e-7, 1e-7
    };

    float sY0[]{
       0,0,0, 0
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
    graph->GetXaxis()->SetLimits(0, 1024);
    graph->GetYaxis()->SetLimits(0.,6.);
    graph->GetYaxis()->SetTitleOffset(2.);
    graph->SetMarkerStyle(3);
    graph->SetMarkerSize(1);
    graph->SetMarkerColor(2);
    graph->Draw("AP");


    TF1 *funz1 = new TF1("funz1","[0]*x + [1]", 0, 1024);
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
     TCanvas *c2 = new TCanvas("c2","Grafico CHN(E)",100,50,800,600);
    c2->SetFillColor(0);
    c2->cd();
    // Istanzio il grafico
    TGraphErrors *graph2 = new TGraphErrors(n, Y, X, sY0, sX);
    graph2->SetMarkerSize(0);
    graph2->SetMarkerStyle(1);
    // Titolo del grafico
    graph2->SetTitle("CHN(E)");
    // Titoli degli assi
    graph2->GetXaxis()->SetTitle("E (MeV)");
    graph2->GetYaxis()->SetTitle("CHN");
    graph->SetMinimum(0);
    graph2->GetXaxis()->SetLimits(0., 1400);
    //graph2->GetYaxis()->SetLimits(0,1400);
    graph2->GetYaxis()->SetTitleOffset(2.);
    graph2->SetMarkerStyle(3);
    graph2->SetMarkerSize(1);
    graph2->SetMarkerColor(2);
    graph2->Draw("AP");


    TF1 *funz2 = new TF1("funz1","[0]*x + [1]", 0., 1400);
    funz2->SetParNames("m", "q");
    cout << "\n\n --- Ipotesi:  " << funz2->GetTitle() << "  ---\n" << endl;
    funz2->SetLineColor(4);
    graph2->Fit("funz1","RM+");

    cout << "Chi^2:" << funz2->GetChisquare() << ", number of DoF: " << funz2->GetNDF() << " (Probability: " << funz2->GetProb() << ")." << endl;

}
