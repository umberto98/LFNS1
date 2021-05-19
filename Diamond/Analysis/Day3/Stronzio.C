#include "Riostream.h"
#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TMath.h"

//CALIBRAZIONE
double calibr = 0.003289; //da moltiplicare nei canali
double offset = 0.007863;
const double gain = 1000.;
const double gaincal = 200.;

const bool halfchan = true; //ci sono 1024 canali

const double gammaen = 0.662; //MeV
const double masselteo = 0.511; //MeV
const double rhopoly = 1.420; //g/cm3
const double energyloss = 1.912; //MeV/g/cm2

TH1D *hcalibr (TH1D *h1); //ricalibra l'istogramma
double calibration (double x) {return offset+calibr*x;}

double beta(double *x, double *par); //singola beta
double betatot(double *x, double *par); //due beta insieme
double tail (double *x, double *par); //coda landau

void Stronzio () {

    ROOT::Math::MinimizerOptions::SetDefaultMaxFunctionCalls(10000);
    ROOT::Math::MinimizerOptions::SetDefaultTolerance(1); 

    //GAIN
    offset = offset*gaincal/gain;
    calibr = calibr*gaincal/gain;

    if (halfchan) {
        offset=2.*offset;
        calibr=2.*calibr;
    }

    TFile *hfile = new TFile("hstronziomax.root");
    TH1D *hold = (TH1D*)hfile->Get("hsr");
    TH1D *hnew = hcalibr(hold);

    hnew->SetMarkerStyle(21);
   	hnew->SetMarkerSize(0.6);
    hnew->GetXaxis()->SetTitle(" E [MeV] ");
    hnew->GetYaxis()->SetTitle("Counts");

    TF1 *fitland = new TF1("fitland","landau",0.22,1.3);
    TF1 *fitbeta1 = new TF1("fitbeta1",beta,0,0.54,4);
    TF1 *fitbeta2 = new TF1("fitbeta2",beta,0,0.9,4); //troncata a 0.8 MeV
    TF1 *fitbeta2tot = new TF1("fitbeta2tot",beta,0,2.29,4); //beta da 2.29 MeV -> Spettro totale -> Serve per norm

    //----------------------------
    //PARAMETRIZZAZIONE DELLE BETA
    //----------------------------

    double a = 10; //numero completamente rndm per inizializzare le const, cioè i parametri 0 delle beta
    double integ1,integ2,integ2tot; //servirà per calcolare gli integrali

    fitbeta1->SetParameters(a,0.511,0.54,0);
    fitbeta2->SetParameters(a,0.511,0.8,0); //la seconda beta sarebbe una distr fino a 2.26 MeV ma viene tagliata a 0.8 -> Approssimo con beta da 0.8 MeV
    fitbeta2tot->SetParameters(a,0.511,2.29,0);

    integ1 = fitbeta1->Integral(0,1.15);
    integ2 = fitbeta2->Integral(0,0.8);
    integ2tot = fitbeta2tot->Integral(0,2.29);
    double weight = integ2/integ2tot;

    fitbeta1->SetParameter(0,fitbeta1->GetParameter(0)/integ1);
    fitbeta2->SetParameter(0,fitbeta2->GetParameter(0)/integ2); //normalizzata a integ2
    fitbeta2tot->SetParameter(0,fitbeta2tot->GetParameter(0)/integ2tot);

    int entries = hnew->GetEntries();

    fitbeta1->SetParameter(0,fitbeta1->GetParameter(0)*entries);
    fitbeta2->SetParameter(0,fitbeta1->GetParameter(0)*0.2);

    fitbeta1->SetTitle(" Beta da 0.54 MeV ");
    fitbeta2->SetTitle(" Beta da 2.29 MeV ");

    fitbeta1->SetLineColor(kOrange);
    fitbeta2->SetLineColor(kGreen);

    hnew->DrawCopy();

    //--------------
    //INIZIO DEL FIT
    //--------------

    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1111);

    //fit landau per inizializzare la landau nella somma
    hnew->Fit(fitland,"RQ0");

    //---------------
    //FIT TOTALE (sx)
    //---------------
    TF1 *fit = new TF1("fit",betatot,0.075,0.55,12); //possiamo fittare con la somma delle 3 solo fino al primo Q value

    //inizializzazione parametri fit totali
    for(int i=0;i<4;i++) fit->SetParameter(i,fitbeta1->GetParameter(i)); //primo beta
    for(int i=0;i<4;i++) fit->SetParameter(i+4,fitbeta2->GetParameter(i)); //secondo beta
    for(int i=0;i<3;i++) fit->SetParameter(i+8,fitland->GetParameter(i)); //landau
    fit->SetParameter(11,5); //costante di bkg

    //limiti
    fit->SetParLimits(0,5500,6500); //la costante della beta
    fit->SetParLimits(1,0.45,0.55); //la massa di e
    fit->SetParLimits(2,0.53,0.58); //Q value della beta
    fit->SetParLimits(3,-0.2,0.2); //traslazione curva
    fit->SetParLimits(4,1000,1700); //la costante della beta 2
    fit->SetParLimits(5,0.45,0.55); //la massa di e
    fit->SetParLimits(6,0.6,0.9); //Q value della beta 2
    fit->SetParLimits(7,-0.2,0.2); //traslazione curva
    fit->SetParLimits(11,0,20); //il bkg totale

    fit->FixParameter(1,0.511); //la massa di e
    fit->FixParameter(5,0.511);

    //singoli contributi
    TF1 *beta1 = new TF1("beta1",beta,0.065,/*0.54*/fit->GetParameter(2),4); //distribuzione della prima beta
    TF1 *beta2 = new TF1("beta2",beta,0.065,/*0.54*/fit->GetParameter(6),4); //distribuzione parziale della seconda beta
    TF1 *land = new TF1("land","landau",0.065,/*0.54*/1.3); //landau

    //fit
    hnew->Fit(fit,"R0"); //fit su funzione totale

    //inizializzazione parametri singoli contributi
    for(int i=0;i<4;i++) beta1->FixParameter(i,fit->GetParameter(i)); //primo beta
    for(int i=0;i<4;i++) beta2->FixParameter(i,fit->GetParameter(i+4)); //secondo beta
    for(int i=0;i<3;i++) land->FixParameter(i,fit->GetParameter(i+8)); //landau

    //titoli
    beta1->SetTitle(" Beta da 0.54 MeV ");
    beta2->SetTitle(" Beta (Parziale) da 2.29 MeV ");
    land->SetTitle(" Landau ");
    fit->SetTitle(" Fit Totale ");

    //colori
    beta1->SetLineColor(kOrange);
    beta2->SetLineColor(kGreen);
    land->SetLineColor(kCyan);

    TCanvas *c1 = new TCanvas();
    hnew->DrawCopy("E1");
    //prima parte
    fit->DrawCopy("same");
    beta1->DrawCopy("same");
    beta2->DrawCopy("same");
    land->DrawCopy("same");
    c1->BuildLegend();

    //-------------------
    //FIT SOMMA CODA (dx) 
    //-------------------
    TF1 *tailfit = new TF1("tailfit",tail,0.54,0.9,8);

    //inizializzo parametri fit
    for(int i=0;i<4;i++) tailfit->SetParameter(i,fit->GetParameter(i+4)); //secondo beta
    for(int i=0;i<3;i++) tailfit->SetParameter(i+4,fit->GetParameter(i+8)); //landau
    tailfit->SetParameter(7,fit->GetParameter(11));
    for(int i=0;i<4;i++) tailfit->SetParLimits(i,fit->GetParameter(i+4)-0.05*fit->GetParameter(i+4),fit->GetParameter(i+4)+0.05*fit->GetParameter(i+4)); //secondo beta
    for(int i=0;i<3;i++) tailfit->SetParLimits(i+4,fit->GetParameter(i+8)-0.05*fit->GetParameter(i+8),fit->GetParameter(i+8)+0.05*fit->GetParameter(i+8)); //landau

    //fit coda
    hnew->Fit(tailfit,"R0");

    //singoli contributi
    TF1 *beta2tail = new TF1("beta2tail",beta,0.54,tailfit->GetParameter(2),4); //seconda beta nella coda
    TF1 *landtail = new TF1("landtail","landau",0.54,1.3); //coda landau
    //fissiamo i parametri
    for(int i=0;i<4;i++) beta2tail->FixParameter(i,tailfit->GetParameter(i)); //contributo coda
    for(int i=0;i<3;i++) landtail->FixParameter(i,tailfit->GetParameter(i+4)); //contributo coda landau
    //titoli
    tailfit->SetTitle(" Fit Coda ");
    beta2tail->SetTitle(" Beta da 2.29 MeV (Stop a 0.8 MeV) ");
    landtail->SetTitle(" Landau (Coda) ");
    //colori
    tailfit->SetLineColor(kRed+2);
    landtail->SetLineColor(kCyan+2);
    beta2tail->SetLineColor(kGreen+2);

    TCanvas *c2 = new TCanvas();
    hnew->DrawCopy("E1");
    //seconda parte
    tailfit->DrawCopy("same");
    beta2tail->DrawCopy("same");
    landtail->DrawCopy("same");
    c2->BuildLegend();

    TCanvas *c3 = new TCanvas();
    hnew->DrawCopy("E1");
    //prima parte
    fit->DrawCopy("same");
    beta1->DrawCopy("same");
    beta2->DrawCopy("same");
    land->DrawCopy("same");
    tailfit->DrawCopy("same");
    beta2tail->DrawCopy("same");
    landtail->DrawCopy("same");
    c3->BuildLegend();

    //-------------------------
    //PERDITA ENERGETICA LANDAU
    //-------------------------

    double energyloss = 1.619; 
    double spess = 500e-4; //500 um in cm
    double rho = 3.51; 
    double spesstil = rho*spess; 
    double zcalc;
    double deltaen = fit->GetParameter(9);
    double sdeltaen = fit->GetParameter(10);
    zcalc = (energyloss*spesstil-deltaen);
    zcalc = zcalc/sdeltaen;

    cout<<" PERDITA ENERGIA LANDAU = ("<<deltaen<<" +- "<<sdeltaen<<") MeV vs "<<energyloss*spesstil<<" Z = "<<zcalc<<endl;


}

TH1D *hcalibr (TH1D *h1) {

    int nbins = h1->GetXaxis()->GetNbins();
    double enmin = offset;
    double enmax = offset+calibr*1024;
    TH1D *hnew = new TH1D("hnew"," Sorgente Stronzio - Gain 1000 ",nbins,enmin,enmax); 

    for (int i=1;i<=nbins;i++) { 

        double y = h1->GetBinContent(i); 
        double x = h1->GetXaxis()->GetBinCenter(i); 
        double yerr = h1->GetBinError(i);
        double xnew = offset+calibr*x;
        hnew->Fill(xnew,y); 
        hnew->SetBinError(i,yerr);

    }

    return hnew;

}

double beta(double *x, double *par) {

    //http://hyperphysics.phy-astr.gsu.edu/hbase/Nuclear/beta2.html#c1

    //PARAMETRI
    //[0] Costante C
    //[1] massa di elettrone
    //[2] Q value
    //[3] shift dato dalla Funzione di Fermi
    //Per le beta - è verso sinistra -> Quindi [3] è negativo

    //if (x[0]>par[2]) par[0] = 0.;

    double f1 = par[0]*TMath::Sqrt((x[0]-par[3])*(x[0]-par[3]) + 2*par[1]*(x[0]-par[3]));
    double f2 = par[2]-x[0]+par[3];
    f2 = f2*f2;
    double f3 = x[0]-par[3]+par[1];

    return f1*f2*f3;

}

double betatot(double *x, double *par) {

    //[0] Costante C
    //[1] massa di elettrone
    //[2] Q value
    //[3] shift dato dalla Funzione di Fermi
    //[4] Costante C
    //[5] massa di elettrone
    //[6] Q value
    //[7] shift dato dalla Funzione di Fermi

    //if (x[0]>par[2]) par[0] = 0.;
    //if (x[0]>par[6]) par[4] = 0.; 

    //prima beta
    double f1 = par[0]*TMath::Sqrt((x[0]-par[3])*(x[0]-par[3]) + 2*par[1]*(x[0]-par[3]));
    double f2 = par[2]-x[0]+par[3];
    f2 = f2*f2;
    double f3 = x[0]-par[3]+par[1];

    //seconda beta
    double f4 = par[4]*TMath::Sqrt((x[0]-par[7])*(x[0]-par[7]) + 2*par[5]*(x[0]-par[7]));
    double f5 = par[6]-x[0]+par[7];
    f5 = f5*f5;
    double f6 = x[0]-par[7]+par[5];

    double ftot = 0.;
    ftot = ftot + f1*f2*f3;
    ftot = ftot + f4*f5*f6;
    ftot = ftot + par[8]*TMath::Landau(x[0],par[9],par[10],0);
    ftot = ftot + par[11];

    return ftot;

}

double tail (double *x, double *par) {

    //[0] Costante C
    //[1] massa di elettrone
    //[2] Q value
    //[3] shift dato dalla Funzione di Fermi

    //seconda beta
    double f1 = par[0]*TMath::Sqrt((x[0]-par[3])*(x[0]-par[3]) + 2*par[1]*(x[0]-par[3]));
    double f2 = par[2]-x[0]+par[3];
    f2 = f2*f2;
    double f3 = x[0]-par[3]+par[1];

    double ftot = f1*f2*f3;
    ftot = ftot + par[4]*TMath::Landau(x[0],par[5],par[6],0);
    ftot = ftot + par[7];

    return ftot;

}