//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Dec 27 16:37:57 2016 by ROOT version 6.08/00
// from TTree EventDST/EventDST
// found on file: outTest.root
//////////////////////////////////////////////////////////

#ifndef BDXDSTSelector_h
#define BDXDSTSelector_h

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

class BDXDSTSelector: public TSelector {
public:

	TTree *fChain = 0;   //!pointer to the analyzed TTree or TChain

	// Readers to access the data
	//TTreeReaderValue<TEvent> event = { fReader, "Event" };

	BDXDSTSelector(TTree * /*tree*/= 0) {
		/*All the histogram pointers MUST be initialized to 0 here*/
		m_EventHeader = 0;
		m_epicsData = 0;
		m_Event = 0;
		isMC = 0;

		Ttot = 0;
		T0 = 0;

		nEventsTotal = 0;
		thisEventN = 0;
		thisEventT = 0;

		hTrigAllEvents = 0;
		hCur1=0;
		hCur2=0;
		hLive1=0;
		hLive2=0;
		hEne1=0;
		hEne2=0;

		hTemperature1=0;
		hTemperature2=0;

		hTrigAllFPGA1=0;
		hTrigAllFPGA2=0;

		hTrigAccFPGA1=0;
		hTrigAccFPGA2=0;
		hEneVsTime=0;
		hEneVsPeakTime=0;

		hTrigBits = 0;
		hTrigBitsPulser = 0;

		hCounter = 0;

		NProof=0;
		runNumber=0;
		eventNumber=0;

		Tbin=300;

		TPeakMin=100;
		TPeakMax=500;
	}
	virtual ~BDXDSTSelector() {
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


	/*Counter histogram for normalization*/
	TH1D *hCounter;

	/*trigger histograms*/
	TH1D *hTrigBits, *hTrigBitsPulser;

	/*Rate-Stability histograms*/
	TH1D *hTrigAllEvents;

	TH1D *hCur1,*hCur2;
	TH1D *hEne1,*hEne2;
	TH1D *hLive1,*hLive2;
	TH1D *hTemperature1,*hTemperature2;
	TH1D* hTrigAllFPGA1,*hTrigAllFPGA2,*hTrigAccFPGA1,*hTrigAccFPGA2;


	TH2D *hEneVsTime;
	TH2D *hEneVsPeakTime;
	/*Variables*/
	double Ttot, T0,Tbin;
	int thisEventN;
	double thisEventT;
	int nEventsTotal;
	int NProof;

	uint64_t thisEventFineTime;

	double TPeakMin,TPeakMax;

	int isMC;

	/*Methods*/
	double getTimeInterval() {
		return Ttot;
	}
	void setTimeInterval(double T) {
		Ttot = T;
		Info("setTimeInterval", Form("Ttot set to %f", Ttot));
	}
	void setTimeBin(double T){
		Tbin = T;
		Info("setTimeBin","Time bin set to %f",Tbin);
	}
	double getT0() {
		return T0;
	}
	void setT0(double T) {
		T0 = T;
		Info("setT0", Form("time0 set to %f", T0));
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

	ClassDef(BDXDSTSelector,1);

};

#endif

#ifdef BDXDSTSelector_cxx
void BDXDSTSelector::Init(TTree *tree)
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

Bool_t BDXDSTSelector::Notify()
{
	// The Notify() function is called when a new file is opened. This
	// can be either for a new TTree in a TChain or when when a new TTree
	// is started when using PROOF. It is normally not necessary to make changes
	// to the generated code, but the routine can be extended by the
	// user if needed. The return value is currently not used.

	return kTRUE;
}

#endif // #ifdef BDXDSTSelector_cxx
