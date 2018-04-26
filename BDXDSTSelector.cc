#define BDXDSTSelector_cxx
// The class definition in BDXDSTSelector.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.

// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// root> T->Process("BDXDSTSelector.C")
// root> T->Process("BDXDSTSelector.C","some options")
// root> T->Process("BDXDSTSelector.C+")
//

#include "BDXDSTSelector.h"
#include <TH2.h>
#include <TStyle.h>
#include <TRandom3.h>
#include <TApplication.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <vector>

void BDXDSTSelector::Begin(TTree * /*tree*/) {
	// The Begin() function is called at the start of the query.
	// When running with PROOF Begin() is only called on the client.
	// The tree argument is deprecated (on PROOF 0 is passed).

	TString option = GetOption();
	isMC = 0;

	if (option.Contains("MC") == true) {
		isMC = 1;
	}

	thisEventFineTime = 0;

}

void BDXDSTSelector::SlaveBegin(TTree * /*tree*/) {
	// The SlaveBegin() function is called after the Begin() function.
	// When running with PROOF SlaveBegin() is called on each slave server.
	// The tree argument is deprecated (on PROOF 0 is passed).
	TString option = GetOption();
	Info("SlaveBegin", "start with options: %s", option.Data());

	int N = (int) (this->Ttot / this->Tbin) + 1;

	isMC = 0;

	if (option.Contains("MC") == true) {
		isMC = 1;
	}

	/*Create here the histograms.*/
	hTrigAllEvents = new TH1D("hTrigAllEvents", "hTrigAllEvents", N, 0, N * Tbin);
	hCur1 = new TH1D("hCur1", "hCur1", N, 0, N * Tbin);
	hCur2 = new TH1D("hCur2", "hCur2", N, 0, N * Tbin);

	hLive1 = new TH1D("hLive1", "hLive1", N, 0, N * Tbin);
	hLive2 = new TH1D("hLive2", "hLive2", N, 0, N * Tbin);

	hEne1 = new TH1D("hEne1", "hEne1", N, 0, N * Tbin);
	hEne2 = new TH1D("hEne2", "hEne2", N, 0, N * Tbin);

	hTrigAllFPGA1 = new TH1D("hTrigAllFGPA1", "hTrigAllFGPA1", N, 0, N * Tbin);
	hTrigAllFPGA2 = new TH1D("hTrigAllFGPA2", "hTrigAllFGPA2", N, 0, N * Tbin);

	hTrigAccFPGA1 = new TH1D("hTrigAccFGPA1", "hTrigAccFGPA1", N, 0, N * Tbin);
	hTrigAccFPGA2 = new TH1D("hTrigAccFGPA2", "hTrigAccFGPA2", N, 0, N * Tbin);

	hTemperature1 = new TH1D("hTemperature1", "hTemperature1", N, 0, N * Tbin);
	hTemperature2 = new TH1D("hTemperature2", "hTemperature2", N, 0, N * Tbin);

	N = this->Ttot / 600;
	hEneVsTime = new TH2D("hEneVsTime", "hEneVsTime", N, 0, this->Ttot, 100, 0, 100);
	hEneVsPeakTime = new TH2D("hEneVsPeakTime", "hEneVsPeakTime", 1000, -10, 2400, 200, 0, 400);

	hQVsTime1Scint5 = new TH2D("hQVsTime1Scint5", "hQVsTime1Scint5", N, 0, this->Ttot, 1000, 0, 50);
	hQVsTime1Scint6 = new TH2D("hQVsTime1Scint6", "hQVsTime1Scint6", N, 0, this->Ttot, 1000, 0, 50);


	hQVsPeakTimeScint5= new TH2D("hQVsPeakTimeScint5","hQVsPeakTimeScint5",1000,-10,2400,500,0,50);
	hQVsPeakTimeScint6= new TH2D("hQVsPeakTimeScint6","hQVsPeakTimeScint6",1000,-10,2400,500,0,50);


	Info("SlaveBegin", "AllHistos to fOutput");
	TIter next(gDirectory->GetList());
	TObject *obj;
	while (obj = (TObject*) next()) {
		if (obj->InheritsFrom(TH1::Class())) {
			fOutput->Add(obj);
		}
	}

	Info("SlaveBegin", "end");
}

Bool_t BDXDSTSelector::Process(Long64_t entry) {
	// The Process() function is called for each entry in the tree (or possibly
	// keyed object in the case of PROOF) to be processed. The entry argument
	// specifies which entry in the currently loaded tree is to be processed.
	// When processing keyed objects with PROOF, the object is already loaded
	// and is available via the fObject pointer.
	//
	// This function should contain the \"body\" of the analysis. It can contain
	// simple or elaborate selection criteria, run algorithms on the data
	// of the event and typically fill histograms.
	//
	// The processing can be stopped by calling Abort().
	//
	// Use fStatus to set the return value of TTree::Process().
	//
	// The return value is currently not used.
	this->GetEntry(entry);

	/*Objects to read collections*/
	CalorimeterHit *fCaloHit;
	IntVetoHit *fIntVetoHit;

	/*Variables*/
	int tWord;
	bool isLateral = false;
	bool isPulser = false;
	bool isCrystal = false;
	/*Get the event header and fill some variables*/
	m_EventHeader = m_Event->getEventHeader();
	m_epicsData = m_EventHeader->getEpicsData();
	eventNumber = m_EventHeader->getEventNumber();
	runNumber = m_EventHeader->getRunNumber();

	if ((m_EventHeader->getTriggerWords()).size() == 0) return kTRUE;
	/*Rate histograms*/
	if (isMC == 0) {
		thisEventT = m_Event->getEventHeader()->getEventTime() - T0;
		hTrigAllEvents->Fill(thisEventT);

		if (m_epicsData->hasData("hac_bcm_average")) {
			//thisEventT= m_epicsData->getDataTime("hac_bcm_average")-T0;
			//	cout<<"1: "<<m_Event->getEventHeader()->getEventTime()<<" "<<m_epicsData->getDataTime("hac_bcm_average")<<" "<<m_epicsData->getDataValue("hac_bcm_average")<<endl;
			hCur1->Fill(thisEventT, m_epicsData->getDataValue("hac_bcm_average"));
			hCur2->Fill(thisEventT);

		}

		if (m_epicsData->hasData("HALLA:p")) {
			//thisEventT= m_epicsData->getDataTime("hac_bcm_average")-T0;
			//cout<<"2: "<<m_Event->getEventHeader()->getEventTime()<<" "<<m_epicsData->getDataTime("HALLA:p")<<" "<<m_epicsData->getDataValue("HALLA:p")<<endl;
			hEne1->Fill(thisEventT, m_epicsData->getDataValue("HALLA:p"));
			hEne2->Fill(thisEventT);

		}

		if (m_epicsData->hasData("BDXarduinoT")) {
			hTemperature1->Fill(thisEventT, m_epicsData->getDataValue("BDXarduinoT"));
			hTemperature2->Fill(thisEventT);
		}

		if (m_epicsData->hasData("B_DET_BDX_FPGA:livetime")) {

			//thisEventT= m_epicsData->getDataTime("B_DET_BDX_FPGA:livetime")-T0;
			hLive1->Fill(thisEventT, m_epicsData->getDataValue("B_DET_BDX_FPGA:livetime"));
			hLive2->Fill(thisEventT);

			hTrigAllFPGA1->Fill(thisEventT, m_epicsData->getDataValue("B_DET_BDX_FPGA_00:cTrg"));
			hTrigAccFPGA1->Fill(thisEventT, m_epicsData->getDataValue("B_DET_BDX_FPGA_01:cTrg"));

		}

		/*Use lateral counters trigger to evaluate crystal gain stability*/
		tWord = m_EventHeader->getTriggerWords()[0];
		if ((tWord >> 1) & 0x1) isLateral = true;
		if ((tWord >> 2) & 0x1) isCrystal = true;
		if ((tWord >> 31) & 0x1) isPulser = true;
		if ((isLateral == false) && (isPulser == false) && (isCrystal == false)) return kTRUE;

		if (isLateral) {
			if (m_Event->hasCollection(CalorimeterHit::Class(), "CalorimeterHits")) {
				TIter CaloHitsIter(m_Event->getCollection(CalorimeterHit::Class(), "CalorimeterHits"));

				while (fCaloHit = (CalorimeterHit*) CaloHitsIter.Next()) { //Need to cast to the proper object
					if ((fCaloHit->m_channel.sector == 0) && (fCaloHit->m_channel.x == 1) && (fCaloHit->m_channel.y == 0)) {
						hEneVsPeakTime->Fill(fCaloHit->T, fCaloHit->E);
						if ((fCaloHit->T > TPeakMin) && (fCaloHit->T < TPeakMax)) {
							hEneVsTime->Fill(thisEventT, fCaloHit->E);
						}
					}
				}
			}
		}
		if (isPulser) {
			if (m_Event->hasCollection(IntVetoHit::Class(), "IntVetoHits")) {
				TIter IntVetoHitsIter(m_Event->getCollection(IntVetoHit::Class(), "IntVetoHits"));

				while (fIntVetoHit = (IntVetoHit*) IntVetoHitsIter.Next()) { //Need to cast to the proper object
					if (fIntVetoHit->m_channel.component == 5) {
						hQVsPeakTimeScint5->Fill(fIntVetoHit->T,fIntVetoHit->Q);
						hQVsTime1Scint5->Fill(thisEventT,fIntVetoHit->Q);
					}
					else if (fIntVetoHit->m_channel.component == 6) {
						hQVsPeakTimeScint6->Fill(fIntVetoHit->T,fIntVetoHit->Q);
						hQVsTime1Scint6->Fill(thisEventT,fIntVetoHit->Q);
					}
				}
			}
		}
	}

	return kTRUE;
}

void BDXDSTSelector::SlaveTerminate() {
// The SlaveTerminate() function is called after all entries or objects
// have been processed. When running with PROOF SlaveTerminate() is called
// on each slave server.

}

void BDXDSTSelector::Terminate() {
// The Terminate() function is the last function to be called during
// a query. It always runs on the client, it can be used to present
// the results graphically or save the results to file.

	Info("Terminate", "Terminate starts");
	Info("Terminate", "Total events are: %i", nEventsTotal);

	TListIter iter(fOutput);
	TObject *obj;
	int binx1, binx2;
	double norm, thr;

	while (obj = iter.Next()) {
		Info("Terminate", "obj: %s", obj->GetName());
	}
	Info("Terminate", "No more objs");

	/*Retrieve here histograms*/
	hCounter = (TH1D*) fOutput->FindObject("hCounter");

	if (isMC == 0) {
		hTrigAllEvents = (TH1D*) fOutput->FindObject("hTrigAllEvents");
		hTrigBits = (TH1D*) fOutput->FindObject("hTrigBits");
		hTrigBitsPulser = (TH1D*) fOutput->FindObject("hTrigBitsPulser");

		hCur1 = (TH1D*) fOutput->FindObject("hCur1");
		hCur2 = (TH1D*) fOutput->FindObject("hCur2");

		hCur1->Divide(hCur2);

		hLive1 = (TH1D*) fOutput->FindObject("hLive1");
		hLive2 = (TH1D*) fOutput->FindObject("hLive2");
		hLive1->Divide(hLive2);

		hEne1 = (TH1D*) fOutput->FindObject("hEne1");
		hEne2 = (TH1D*) fOutput->FindObject("hEne2");
		hEne1->Divide(hEne2);

		hTemperature1 = (TH1D*) fOutput->FindObject("hTemperature1");
		hTemperature2 = (TH1D*) fOutput->FindObject("hTemperature2");
		hTemperature1->Divide(hTemperature2);

		hTrigAllFPGA1 = (TH1D*) fOutput->FindObject("hTrigAllFGPA1");
		hTrigAccFPGA1 = (TH1D*) fOutput->FindObject("hTrigAccFGPA1");

		hTrigAccFPGA1->Divide(hLive2);
		hTrigAllFPGA1->Divide(hLive2);

		hEneVsTime = (TH2D*) fOutput->FindObject("hEneVsTime");
		hEneVsPeakTime = (TH2D*) fOutput->FindObject("hEneVsPeakTime");

		hQVsTime1Scint5 = (TH2D*) fOutput->FindObject("hQVsTime1Scint5");
		hQVsTime1Scint6 = (TH2D*) fOutput->FindObject("hQVsTime1Scint6");

		hQVsPeakTimeScint5= (TH2D*) fOutput->FindObject("hQVsPeakTimeScint5");
		hQVsPeakTimeScint6= (TH2D*) fOutput->FindObject("hQVsPeakTimeScint6");
	}

	/*Rate histos*/
	if (isMC == 0) {
		/*Stability histograms*/
		hTrigAllEvents->Sumw2();
		hTrigAllEvents->Scale(1., "width");
	}

}

