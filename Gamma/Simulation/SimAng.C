#include "Riostream.h"
#include "TRandom3.h"
#include "TFile.h"
#include "TH1D.h"
#include "TMath.h"
#include "TLatex.h"

using namespace TMath;

const double rad = 2.54; //raggio parte attiva scintillatore
const double d1 = 9.35; //distanza primo scintillatore da sorgente
const double d2 = 9.35; //distanza secondo scintillatore da sorgente
const double offset = 0; //leggendo 1° gli scintillatori erano allineati

const int kSim = 1e7; //eventi simulati

double atan4(double y, double x); //arcotangente
bool checkscint(double x, double y, double ang); //check se scintillatore viene colpito

//------------------------------------------------------
//SIMULAZIONE COINCIDENZE ANGOLARI - SPETTROSCOPIA GAMMA
//------------------------------------------------------

//---------------------------------------------------------
//CENTRO S.R. NELLA SORGENTE
//VISTA DALL'ALTO ABBIAMO Z USCENTE
//X INDICA LA COORDINATA PARALLELA ALLA DISTANZA DI S FIXED
//---------------------------------------------------------

void SimAng () {

    TFile *simfile = new TFile("hsim.root","recreate");

    double step = 1; //1 grado di step
    double angmin = -40.; 
    double angmax = 40.;
    angmin = angmin + offset;
    angmax = angmax + offset;
    int bins = (int)((angmax-angmin)/step);
    TH1D *hsim = new TH1D("hsim","SIMULAZIONE COINCIDENZE",bins+1,angmin-step/2.,angmax+step/2.);
    hsim->GetXaxis()->SetTitle(" Angolo [#circ] ");
    hsim->GetYaxis()->SetTitle(" Coincidenze [SIM] ");

    for (double ang = angmin; ang<=angmax; ang=ang+step) { //loop su angoli

        cout<<"\n --------------- \n Angolo = "<<ang<<" ° \n --------------- \n";

        for(int i=0;i<kSim;i++) { //loop su eventi

            if (i%1000000==0) cout<<" Evento "<<i<<endl;

            //---------------------------------
            //PUNTO INIZIALE SU SCINTILLATORE 1
            //---------------------------------

            double x0 = -d1;
            double rad1 = rad*gRandom->Rndm(); //numero fra 0 e rad
            double phi = 2*Pi()*gRandom->Rndm(); //numero fra 0 e 2pi
            double y0 = rad1*Cos(phi);
            double z0 = rad1*Sin(phi);

            //------------
            //PUNTO FINALE
            //------------

            if (phi<Pi()) phi=phi+Pi();
            else phi = phi-Pi();

            double x1 = d2;
            double y1 = rad1*Cos(phi);
            double z1 = rad1*Sin(phi);

            if (checkscint(x1,y1,DegToRad()*ang)) hsim->Fill(ang+offset);

        } //fine loop su eventi

    } //fine loop su angoli

    hsim->Scale(1./kSim);
    hsim->Draw("hist");
    simfile->Write();

}

double atan4(double y, double x) {

    if (x==0) {
        if (y>=0) return (Pi())/2.; //semiasse positivo di y
        if (y<0) return Pi()*3/2; //semiasse negativo di y
    }

    if (x>0 && y>=0) {
        return ATan(y/x);
    } //primo quadrante, l'arctotangente è già tra 0 e pi/2

    if (x<0 && y>=0) {
        return ATan(y/x) + Pi();
    } //secondo quadrante, l'arcotangente è fra -pi/2 e 0, va aggiunto pi

    if (x<0 && y<=0) {
        return ATan(y/x) + Pi();
    } //terzo quadrante, l'arcotangente è fra 0 e pi/2, va aggiunto pi 

    if (x>0 && y<=0) {
        return ATan(y/x) +2*Pi();
    } //quarto quadrante, l'arcotangente è fra -pi/2 e 0, vanno aggiunti 2pi

    else {
        cout <<"C'è qualcosa di errato!"<<endl;
        return -500;
    }
}

bool checkscint(double x, double y, double ang) {

    if (ang==0.) return true;

    ang = Abs(ang);

    double alpha = atan4(rad,d2);
    double delta = rad*rad + d2*d2;
    delta = Sqrt(delta);

    double xmin = delta*Cos(ang+alpha);
    double xmax = delta*Cos(ang-alpha);

    double ymax = delta*Sin(ang+alpha);
    double ymin = delta*Sin(ang-alpha);

    if (x>=xmin && x<=xmax && y>=ymin && y<=ymax) return true;

    return false;

}


