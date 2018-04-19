#define BDXDSTSelector2_cxx
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

#include "BDXDSTSelector2.h"
#include <TH2.h>
#include <TStyle.h>
#include <TRandom3.h>
#include <TApplication.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <vector>

void BDXDSTSelector2::Begin(TTree * /*tree*/) {
	// The Begin() function is called at the start of the query.
	// When running with PROOF Begin() is only called on the client.
	// The tree argument is deprecated (on PROOF 0 is passed).

	TString option = GetOption();

	thisEventFineTime = 0;

}

void BDXDSTSelector2::SlaveBegin(TTree * /*tree*/) {
	// The SlaveBegin() function is called after the Begin() function.
	// When running with PROOF SlaveBegin() is called on each slave server.
	// The tree argument is deprecated (on PROOF 0 is passed).
	TString option = GetOption();
	Info("SlaveBegin", "start with options: %s", option.Data());

	int N = (int) (this->Ttot / this->Tbin) + 1;

	/*Create here the histograms.*/
	hTrigAllEventsBeam = new TH1D("hTrigAllEventsBeam", "hTrigAllEventsBeam", N, 0, N * Tbin);
	hTrigAllEventsCosmics = new TH1D("hTrigAllEventsCosmics", "hTrigAllEventsCosmics", N, 0, N * Tbin);

	hEneCrystalBeamTrg4 = new TH1D("hEneCrystalBeamTrg4", "hEneCrystalBeamTrg4", 200, 0, 2000);
	hEneCrystalCosmicsTrg4 = new TH1D("hEneCrystalCosmicsTrg4", "hEneCrystalCosmicsTrg4", 200, 0, 2000);

	hEneCrystalBeamTrg2 = new TH1D("hEneCrystalBeamTrg2", "hEneCrystalBeamTrg2", 200, 0, 2000);
	hEneCrystalCosmicsTrg2 = new TH1D("hEneCrystalCosmicsTrg2", "hEneCrystalCosmicsTrg2", 200, 0, 2000);

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

Bool_t BDXDSTSelector2::Process(Long64_t entry) {
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
	int ibin;
	bool isCrystalTrg4 = false;
	bool isCrystalTrg2 = false;

	int eventType;

	double eneCorr;

	/*Get the event header and fill some variables*/
	m_EventHeader = m_Event->getEventHeader();
	m_epicsData = m_EventHeader->getEpicsData();
	eventNumber = m_EventHeader->getEventNumber();
	runNumber = m_EventHeader->getRunNumber();

	if (m_EventHeader == 0) return kTRUE;
	if (m_EventHeader->getTriggerWords().size() == 0) return kTRUE;
	thisEventT = m_Event->getEventHeader()->getEventTime() - T0;

	/*Check the type of this event: beam, cosmics, none*/
	ibin = hTimeIntervals->FindBin(thisEventT);
	eventType = hTimeIntervals->GetBinContent(ibin);
	if (eventType == 0) return kTRUE;

	/*Check the energy correction*/
	ibin = hEnergyCorrection->FindBin(thisEventT);
	eneCorr = hEnergyCorrection->GetBinContent(ibin);
	if (eneCorr <= 0) return kTRUE;

	if (m_epicsData->hasData("hac_bcm_average")) {
		//thisEventT= m_epicsData->getDataTime("hac_bcm_average")-T0;
		//	cout<<"1: "<<m_Event->getEventHeader()->getEventTime()<<" "<<m_epicsData->getDataTime("hac_bcm_average")<<" "<<m_epicsData->getDataValue("hac_bcm_average")<<endl;

	}

	if (m_epicsData->hasData("HALLA:p")) {
		//thisEventT= m_epicsData->getDataTime("hac_bcm_average")-T0;
		//cout<<"2: "<<m_Event->getEventHeader()->getEventTime()<<" "<<m_epicsData->getDataTime("HALLA:p")<<" "<<m_epicsData->getDataValue("HALLA:p")<<endl;

	}

	if (m_epicsData->hasData("B_DET_BDX_FPGA:livetime")) {

		//thisEventT= m_epicsData->getDataTime("B_DET_BDX_FPGA:livetime")-T0;

	}

	tWord = m_EventHeader->getTriggerWords()[0];
	if ((tWord >> 4) & 0x1) isCrystalTrg4 = true;
	if ((tWord >> 2) & 0x1) isCrystalTrg2 = true;

	/*BEAM*/
	switch (eventType) {
	case 1:
		hTrigAllEventsBeam->Fill(thisEventT);
		break;
	case 2:
		hTrigAllEventsCosmics->Fill(thisEventT);
		break;

	}

	if ((isCrystalTrg4 == false) && (isCrystalTrg2 == false)) return kTRUE;

	if (m_Event->hasCollection(CalorimeterHit::Class(), "CalorimeterHits")) {
		TIter CaloHitsIter(m_Event->getCollection(CalorimeterHit::Class(), "CalorimeterHits"));
		while (fCaloHit = (CalorimeterHit*) CaloHitsIter.Next()) { //Need to cast to the proper object
			if ((fCaloHit->m_channel.sector == 0) && (fCaloHit->m_channel.x == 1) && (fCaloHit->m_channel.y == 0)) {
				switch (eventType) {
				case 1:
					if (isCrystalTrg4) hEneCrystalBeamTrg4->Fill(fCaloHit->E * eneCorr);
					if (isCrystalTrg2) hEneCrystalBeamTrg2->Fill(fCaloHit->E * eneCorr);
					break;
				case 2:
					if (isCrystalTrg4) 	hEneCrystalCosmicsTrg4->Fill(fCaloHit->E * eneCorr);
					if (isCrystalTrg2)  hEneCrystalCosmicsTrg2->Fill(fCaloHit->E * eneCorr);
					break;
				}
			}
		}
	}

	return kTRUE;
}

void BDXDSTSelector2::SlaveTerminate() {
// The SlaveTerminate() function is called after all entries or objects
// have been processed. When running with PROOF SlaveTerminate() is called
// on each slave server.

}

void BDXDSTSelector2::Terminate() {
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

	hTrigAllEventsBeam = (TH1D*) fOutput->FindObject("hTrigAllEventsBeam");
	hTrigAllEventsCosmics = (TH1D*) fOutput->FindObject("hTrigAllEventsCosmics");

	hEneCrystalBeamTrg4 = (TH1D*) fOutput->FindObject("hEneCrystalBeamTrg4");
	hEneCrystalCosmicsTrg4 = (TH1D*) fOutput->FindObject("hEneCrystalCosmicsTrg4");

	hEneCrystalBeamTrg2 = (TH1D*) fOutput->FindObject("hEneCrystalBeamTrg2");
	hEneCrystalCosmicsTrg2 = (TH1D*) fOutput->FindObject("hEneCrystalCosmicsTrg2");

	hTrigAllEventsBeam->Sumw2();
	hTrigAllEventsBeam->Scale(1., "width");

	hTrigAllEventsCosmics->Sumw2();
	hTrigAllEventsCosmics->Scale(1., "width");

}

