#include <stdio.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "t1.h"

#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TCanvas.h> 
#include <TLorentzVector.h>



//------------------------------------------------------------------------------
// Particle Class
//
class Particle{

	public:
	Particle();
	// FIXME : Create an additional constructor that takes 4 arguments --> the 4-momentum
	Particle(double, double, double, double); // four doubles for energy and 3-momentum
	double   pt, eta, phi, E, m, p[4];
	void     p4(double, double, double, double);
	void     print();
	void     setMass(double);
	double   sintheta();
};

//------------------------------------------------------------------------------

//*****************************************************************************
//                                                                             *
//    MEMBERS functions of the Particle Class                                  *
//                                                                             *
//*****************************************************************************

//
//*** Default constructor ------------------------------------------------------
//
Particle::Particle(){
	pt = eta = phi = E = m = 0.0;
	p[0] = p[1] = p[2] = p[3] = 0.0;
}

//*** Additional constructor ------------------------------------------------------
Particle::Particle(double p0, double p1, double p2, double p3){ 
	//FIXME
	p[0] = p0;
	p[1] = p1; // x
	p[2] = p2; // y
	p[3] = p3; // z, beam axis
	E = p0;
	pt = TMath::Sqrt(p1*p1 + p2*p2);
	double pmag = TMath::Sqrt(pt*pt + p3*p3);
	eta = 0.5*TMath::Log((pmag + p3) / (pmag - p3));
	phi = TMath::ATan2(p2, p1);
}

//
//*** Members  ------------------------------------------------------
//
double Particle::sintheta(){
	//FIXME
	return TMath::Sin(2*TMath::ATan(TMath::Exp(-eta)));
}

void Particle::p4(double pT, double eta, double phi, double energy){
	//FIXME
	//Assuming p4 is computing and saving four-momentum vector
	p[0] = energy;
	p[1] = pT*TMath::Cos(phi);
	p[2] = pT*TMath::Sin(phi);
	double tan_eta = TMath::Tan(eta);
	double tan_eta2 = tan_eta*tan_eta;
	p[3] = (1 - tan_eta2) / (tan_eta2*pT*pT);
}

void Particle::setMass(double mass)
{
	// FIXME
	// Assuming that the mass determines something about the four momentum
	p[0] = mass*mass + p[1]*p[1] + p[2]*p[2] + p[3]*p[3];
}

//
//*** Prints 4-vector ----------------------------------------------------------
//
void Particle::print(){
	std::cout << std::endl;
	std::cout << "(" << p[0] <<",\t" << p[1] <<",\t"<< p[2] <<",\t"<< p[3] << ")" << "  " <<  sintheta() << std::endl;
}

class Lepton: public Particle {
	public:
	void print();
	int charge; // either +1 or -1
	void set_charge(int);
};

void Lepton::set_charge(int charge) {
	charge = charge;
}

void Lepton::print(){
	std::cout << std::endl;
	std::cout << "(" << p[0] <<",\t" << p[1] <<",\t"<< p[2] <<",\t"<< p[3] << ")" << "  " <<  sintheta() << " " << charge << std::endl;
}

class Jet: public Particle {
	public:
	void print();
	int flavor; // 0, 4, 5
	void set_flavor(int);
};

void Jet::set_flavor(int flavor) {
	flavor = flavor;
}

void Jet::print(){
	std::cout << std::endl;
	std::cout << "(" << p[0] <<",\t" << p[1] <<",\t"<< p[2] <<",\t"<< p[3] << ")" << "  " <<  sintheta() << " " << flavor << std::endl;
}

int main() {
	
	/* ************* */
	/* Input Tree   */
	/* ************* */

	TFile *f      = new TFile("input.root","READ");
	TTree *t1 = (TTree*)(f->Get("t1"));

	// Read the variables from the ROOT tree branches
	t1->SetBranchAddress("lepPt",&lepPt);
	t1->SetBranchAddress("lepEta",&lepEta);
	t1->SetBranchAddress("lepPhi",&lepPhi);
	t1->SetBranchAddress("lepE",&lepE);
	t1->SetBranchAddress("lepQ",&lepQ);
	
	t1->SetBranchAddress("njets",&njets);
	t1->SetBranchAddress("jetPt",&jetPt);
	t1->SetBranchAddress("jetEta",&jetEta);
	t1->SetBranchAddress("jetPhi",&jetPhi);
	t1->SetBranchAddress("jetE", &jetE);
	t1->SetBranchAddress("jetHadronFlavour",&jetHadronFlavour);

	// Total number of events in ROOT tree
	Long64_t nentries = t1->GetEntries();

	for (Long64_t jentry=0; jentry<100;jentry++)
 	{
		t1->GetEntry(jentry);
		std::cout<<" Event "<< jentry <<std::endl;	

		//FIX ME
		// loop over leptons
		nleps = sizeof(lepE) / sizeof(lepE[0]);
		if (nleps > maxLepSize) nleps = maxLepSize;
		for (int i = 0; i < nleps; i++) {
			Lepton lepton = Lepton();
			lepton.p4(lepPt[i], lepEta[i], lepPhi[i], lepE[i]);
			lepton.set_charge(lepQ[i]);
			lepton.print();
		}

		// loop over leptons
		njets = sizeof(jetE) / sizeof(jetE[0]);
		if (njets > maxLepSize) njets = maxLepSize;
		for (int i = 0; i < njets; i++) {
			Jet jet = Jet();
			jet.p4(jetPt[i], jetEta[i], jetPhi[i], jetE[i]);
			jet.set_flavor(jetHadronFlavour[i]);
			jet.print();
		}

	} // Loop over all events

  	return 0;
}
