//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Dec 27 16:37:57 2016 by ROOT version 6.08/00
// from TTree EventDST/EventDST
// found on file: outTest.root
//////////////////////////////////////////////////////////

#ifndef BDXDSTSelector2_h
#define BDXDSTSelector2_h

#include <TROOT.h>
#include "TProofOutputFile.h"
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <TClonesArray.h>
#include <TCollection.h>

#include <TH1D.h>
#include <TH2D.h>
#include <TAxis.h>
#include <TCanvas.h>
#include <TObjArray.h>
// Headers needed by this particular selector
#include "EventBuilder/TEvent.h"
#include "EventBuilder/TEventHeader.h"

#include "IntVeto/IntVetoDigiHit.h"
#include "Calorimeter/CalorimeterDigiHit.h"

#include "Calorimeter/CalorimeterHit.h"
#include "IntVeto/IntVetoHit.h"
#include "ExtVeto/ExtVetoHit.h"

#include "Calorimeter/CalorimeterMCRealHit.h"

#include "EventBuilder/TEventHeader.h"
#include "EPICS/epicsData.h"

class BDXDSTSelector2: public TSelector {
public:

	TTree *fChain = 0;   //!pointer to the analyzed TTree or TChain

	// Readers to access the data
	//TTreeReaderValue<TEvent> event = { fReader, "Event" };

	BDXDSTSelector2(TTree * /*tree*/= 0) {
		/*All the histogram pointers MUST be initialized to 0 here*/
		m_EventHeader = 0;
		m_epicsData = 0;
		m_Event = 0;

		Ttot = 0;
		T0 = 0;

		nEventsTotal = 0;
		thisEventN = 0;
		thisEventT = 0;

		hTrigAllEventsBeam = 0;
		hTrigAllEventsBeam2 = 0;
		hTrigAllEventsCosmics = 0;
		hTimeIntervals = 0;
		hEnergyCorrection = 0;
		hTimeBinID=0;
		nTimeBins=0;

		hEneCrystalBeamTrg4 = 0;
		hEneCrystalBeam2Trg4 = 0;
		hEneCrystalCosmicsTrg4 = 0;

		hEneCrystalBeamTrg2 = 0;
		hEneCrystalBeam2Trg2 = 0;
		hEneCrystalCosmicsTrg2 = 0;



		hEneNoScintCrystalBeamTrg4 = 0;
		hEneNoScintCrystalBeam2Trg4 = 0;
		hEneNoScintCrystalCosmicsTrg4 = 0;

		hEneNoScintCrystalBeamTrg2 = 0;
		hEneNoScintCrystalBeam2Trg2 = 0;
		hEneNoScintCrystalCosmicsTrg2 = 0;


		hEneCrystalVsQScint5 = 0;
		hEneCrystalVsQScint6 = 0;

		NProof = 0;
		runNumber = 0;
		eventNumber = 0;

		Tbin = 300;

		TPeakMin = 100;
		TPeakMax = 500;

		hEneVsPeakTimeTrg2 = 0;

		QScintThr = 50;

	}
	virtual ~BDXDSTSelector2() {
	}
	virtual Int_t Version() const {
		return 2;
	}
	virtual void Begin(TTree *tree);
	virtual void SlaveBegin(TTree *tree);
	virtual void Init(TTree *tree);
	virtual Bool_t Notify();
	virtual Bool_t Process(Long64_t entry);
	virtual Int_t GetEntry(Long64_t entry, Int_t getall = 0) {
		return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0;
	}
	virtual void SetOption(const char *option) {
		fOption = option;
	}
	virtual void SetObject(TObject *obj) {
		fObject = obj;
	}
	virtual void SetInputList(TList *input) {
		fInput = input;
	}
	virtual TList *GetOutputList() const {
		return fOutput;
	}
	virtual void SlaveTerminate();
	virtual void Terminate();

	int eventNumber, runNumber;

	/*Pointers. Note that EACH histogram pointer MUST be initialized to 0 in the TSelector constructor*/
	TEventHeader *m_EventHeader;
	TEvent *m_Event;
	epicsData *m_epicsData;

	TH1D *hTimeIntervals;
	TH1D *hEnergyCorrection;

	int nTimeBins;
	TH1D *hTimeBinID;
	vector<TH1D*> hTimeBins;


	TH1D *hTrigAllEventsBeam;
	TH1D *hTrigAllEventsBeam2;
	TH1D *hTrigAllEventsCosmics;

	TH1D *hEneCrystalBeamTrg2;
	TH1D *hEneCrystalBeam2Trg2;
	TH1D *hEneCrystalCosmicsTrg2;

	TH1D *hEneCrystalBeamTrg4;
	TH1D *hEneCrystalBeam2Trg4;
	TH1D *hEneCrystalCosmicsTrg4;

	TH1D *hEneNoScintCrystalBeamTrg2;
	TH1D *hEneNoScintCrystalBeam2Trg2;
	TH1D *hEneNoScintCrystalCosmicsTrg2;

	TH1D *hEneNoScintCrystalBeamTrg4;
	TH1D *hEneNoScintCrystalBeam2Trg4;
	TH1D *hEneNoScintCrystalCosmicsTrg4;

	TH2D *hEneVsPeakTimeTrg2;

	TH2D *hEneCrystalVsQScint5;
	TH2D *hEneCrystalVsQScint6;

	/*Variables*/
	double Ttot, T0, Tbin;
	int thisEventN;
	double thisEventT;
	int nEventsTotal;
	int NProof;

	uint64_t thisEventFineTime;
	double TPeakMin, TPeakMax;
	double QScintThr;


	/*Methods*/
	double getTimeInterval() {
		return Ttot;
	}
	void setTimeInterval(double T) {
		Ttot = T;
		Info("setTimeInterval", Form("Ttot set to %f", Ttot));
	}
	void setTimeBin(double T) {
		Tbin = T;
		Info("setTimeBin", "Time bin set to %f", Tbin);
	}
	double getT0() {
		return T0;
	}
	void setT0(double T) {
		T0 = T;
		Info("setT0", Form("time0 set to %f", T0));
	}

	void sethTimeIntervals(TH1D *h) {
		hTimeIntervals = h;
	}

	void sethTimeBinID(TH1D *h,int nBins){
		hTimeBinID=h;
		nTimeBins=nBins;
	}

	void sethEnergyCorrection(TH1D *h) {
		hEnergyCorrection = h;
	}

	void setNProof(int N) {
		NProof = N;
	}
	double getPeakMax() const {
		return TPeakMax;
	}

	void setPeakMax(double peakMax) {
		TPeakMax = peakMax;
	}

	double getPeakMin() const {
		return TPeakMin;
	}

	void setPeakMin(double peakMin) {
		TPeakMin = peakMin;
	}

	double getScintThr() const {
		return QScintThr;
	}

	void setScintThr(double scintThr) {
		QScintThr = scintThr;
	}

ClassDef(BDXDSTSelector2,1)
	;

};

#endif

#ifdef BDXDSTSelector2_cxx
void BDXDSTSelector2::Init(TTree *tree)
{
	// The Init() function is called when the selector needs to initialize
	// a new tree or chain. Typically here the reader is initialized.
	// It is normally not necessary to make changes to the generated
	// code, but the routine can be extended by the user if needed.
	// Init() will be called many times when running on PROOF
	// (once per file to be processed).
	if (!tree) return;
	fChain = tree;

	fChain->SetBranchAddress("Event", &m_Event);

}

Bool_t BDXDSTSelector2::Notify()
{
	// The Notify() function is called when a new file is opened. This
	// can be either for a new TTree in a TChain or when when a new TTree
	// is started when using PROOF. It is normally not necessary to make changes
	// to the generated code, but the routine can be extended by the
	// user if needed. The return value is currently not used.

	return kTRUE;
}

#endif // #ifdef BDXDSTSelector2_cxx
