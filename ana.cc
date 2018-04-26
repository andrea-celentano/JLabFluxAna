#include "BDXDSTSelector.h"
#include "BDXDSTSelector2.h"
#include "TTree.h"
#include "TFile.h"

#include <iostream>
#include <string>
#include "TApplication.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TProof.h"
#include "TF1.h"
#include "TVectorD.h"
#include "defs.h"

#include "RooRealVar.h"
#include "RooPlot.h"
#include "RooArgSet.h"
#include "RooDataSet.h"
#include "RooGaussian.h"
#include "RooLandau.h"
#include "RooNumConvPdf.h"
#include "RooAddPdf.h"
#include "RooAbsPdf.h"
#include "RooFFTConvPdf.h"
#include "RooPolynomial.h"
#include "RooExponential.h"
#include "RooArgusBG.h"
#include "RooGenericPdf.h"
using namespace std;
using namespace RooFit;

int nproof;
int Ntot;
int N0;

bool showGUI, doTree;

string ofname, fname;
vector<string> fnames;

void parseCommandLine(int argc, char **argv) {

	nproof = 0;
	Ntot = 0;
	N0 = 0;
	ofname = "";

	showGUI = false;

	for (int ii = 0; ii < argc; ii++) {
		string command = string(argv[ii]);
		if (command == "-f") {
			for (int jj = ii + 1; jj < argc; jj++) {
				fname = string(argv[jj]);
				if (fname[0] == '-') break;
				cout << "fname is: " << fname << endl;
				fnames.push_back(fname);
			}
		}
		if (command == "-o") {
			ofname = string(argv[ii + 1]);
			cout << "out fname is: " << ofname << endl;
		} else if (command == "-nproof") {
			nproof = atoi(argv[ii + 1]);
		} else if (command == "-Ntot") {
			Ntot = atoi(argv[ii + 1]);
		} else if (command == "-GUI")
			showGUI = true;
		else if (command == "-tree") doTree = true;
	}
}

int main(int argc, char **argv) {

	TApplication gui("GUI", 0, NULL);
	parseCommandLine(argc, argv);

	const double beamCurrentMin = 8;
	const double beamEnergyMin = 10E3; //MeV
	const double beamLivetimeMin = 90;

	const double beamCurrentMin2 = 8;
	const double beamEnergyMin2 = 4E3; //MeV
	const double beamEnergyMax2 = 4.5E3; //MeV
	const double beamLivetimeMin2 = -100;

	const double cosmicsCurrentMax = .1;
	const double cosmicsLivetimeMin = -100; /// 90;

	const double cosmicsTimeWidth = 7200; //seconds - 2 hours bin

	const double landauMeanMin = 10;
	const double landauMeanMax = 40;

	const double landauMeanNominal = 30;

	const double TPeakMin = 100;
	const double TPeakMax = 500;

	const double QScintThr = 50;

	int doFit = 0;

	string opt = "";

	if (doTree) opt = "Tree:" + opt;

	BDXDSTSelector *myBDXDSTSelector = new BDXDSTSelector();
	BDXDSTSelector2 *myBDXDSTSelector2 = new BDXDSTSelector2();

	TChain *runInfo = new TChain("RunInfo");
	TChain *DSTChain = new TChain("EventDST");

	for (int jj = 0; jj < fnames.size(); jj++) {
		DSTChain->Add(fnames[jj].c_str());
		runInfo->Add(fnames[jj].c_str());
	}

	int N = DSTChain->GetEntries();
	if (Ntot <= 0) Ntot = N;
	cout << "Before process: N is " << N << endl;

	/*Determine total run time*/
	double Ttot, Ttot2;
	int Tfirst, Tlast;
	int runN, dT;
	runInfo->SetBranchAddress("runN", &runN);
	runInfo->SetBranchAddress("dT", &dT);

	double timeBin = 30;

	Ttot = 0;
	for (int irun = 0; irun < runInfo->GetEntries(); irun++) {
		runInfo->GetEntry(irun);
		printf("Run %i: %i\n", runN, dT);
		Ttot += dT;
	}

	/*Determine first event time*/
	cout << "Determine first/last event time " << endl;
	TEvent *event_tmp = 0;
	DSTChain->SetBranchAddress("Event", &event_tmp);
	DSTChain->GetEntry(N0);
	Tfirst = event_tmp->getEventHeader()->getEventTime();
	cout << "setting time interval from data: " << Ttot << endl;
	myBDXDSTSelector->setTimeInterval(Ttot);
	cout << "setting first event time: " << Tfirst << endl;
	myBDXDSTSelector->setT0(1. * Tfirst);
	myBDXDSTSelector->setTimeBin(timeBin); //5 minutes per bin
	myBDXDSTSelector->setNProof(nproof);

	myBDXDSTSelector->setPeakMin(TPeakMin);
	myBDXDSTSelector->setPeakMax(TPeakMax);

	if (nproof != 0) {
		cout << "USING PROOF WITH " << nproof << " WORKERS! " << endl;
		TProof *proof = TProof::Open(Form("workers=%i", nproof));

		proof->Exec("gSystem->Load(\"libRooFit.so\")");
		proof->Exec("gSystem->Load(\"${BDXRECO_ROOT}/lib/libJANA.so\")");
		proof->Exec("gSystem->Load(\"libbdxRecoExt.so\")");
		proof->Exec("gSystem->Load(\"libbdxReco.so\")");
		proof->Exec("gSystem->Load(\"/mnt_work/apcx4/celentano/BeamDump/JLabFlux/JLabFluxAna/libBDXDSTSelector.so\")");
		proof->SetParameter("PROOF_Packetizer", "TPacketizer");
		DSTChain->SetProof(1);
	}

	myBDXDSTSelector->nEventsTotal = Ntot;
	DSTChain->Process(myBDXDSTSelector, opt.c_str(), Ntot, N0);

	cout << "After process " << endl;
	TH1D *hTimeIntervals;
	TH1D *hTimeBinID;
	TH2D *hEneVsTime;
	TH1D *hEneCorrection;
	TH1D *hEneMean;

	/*Here I determine the different time intervals:
	 * BEAM:
	 * ** ENERGY > 10 GeV
	 * ** CURRENT > 8 uA
	 * ** LIVETIME > 90%
	 *
	 * COSMICS:
	 * ** ANY ENERGY
	 * ** CURRENT < 0.1 uA
	 * ** LIVETIME >
	 */

	double Tbeam = 0;
	double Qbeam = 0;
	double Tbeam2 = 0;
	double Qbeam2 = 0;
	double Tcosmics = 0;

	hTimeIntervals = (TH1D*) myBDXDSTSelector->hCur1->Clone("hTimeIntervals");
	hTimeIntervals->SetTitle("hTimeIntervals");
	hTimeIntervals->Reset();

	hTimeBinID = (TH1D*) myBDXDSTSelector->hCur1->Clone("hTimeBinID");
	hTimeBinID->SetTitle("hTimeBinID");
	hTimeBinID->Reset();

	cout << hTimeIntervals->GetNbinsX() << endl;

	/*In order to be conservative, the time-intervals analysis proceeds in this way:
	 * 1. I loop over all the bins in hCur1, hLive1, hEne1 histograms
	 * 2. For a given bin to be considered "cosmics" or "beam", that beam AND THE PREVIOUS ONE must satisfy the requirements
	 * 3. The most efficient way to proceed is thus to loop from the maximum bin to the minimum one
	 *
	 */
	double current, energy, livetime;
	double currentPrev, energyPrev, livetimePrev;
	for (int ibin = hTimeIntervals->GetNbinsX(); ibin >= 1; ibin--) {
		current = myBDXDSTSelector->hCur1->GetBinContent(ibin);
		energy = myBDXDSTSelector->hEne1->GetBinContent(ibin);
		livetime = myBDXDSTSelector->hLive1->GetBinContent(ibin);

		currentPrev = myBDXDSTSelector->hCur1->GetBinContent(ibin - 1);
		energyPrev = myBDXDSTSelector->hEne1->GetBinContent(ibin - 1);
		livetimePrev = myBDXDSTSelector->hLive1->GetBinContent(ibin - 1);

		cout << "ibin " << ibin << " " << current << " " << energy << " " << livetime << endl;

		/*BEAM-1*/
		if ((current > beamCurrentMin) && (energy > beamEnergyMin) && (livetime > beamLivetimeMin) && (currentPrev > beamCurrentMin) && (energyPrev > beamEnergyMin) && (livetimePrev > beamLivetimeMin)) {
			hTimeIntervals->SetBinContent(ibin, eventType::beam_11GeV);
			Tbeam += hTimeIntervals->GetBinWidth(ibin);
			Qbeam += hTimeIntervals->GetBinWidth(ibin) * current; //uC
		}

		/*BEAM-2*/
		else if ((current > beamCurrentMin2) && (energy > beamEnergyMin2) && (energy < beamEnergyMax2) && (livetime > beamLivetimeMin2) && (currentPrev > beamCurrentMin2) && (energyPrev > beamEnergyMin2) && (energyPrev < beamEnergyMax2) && (livetimePrev > beamLivetimeMin2)) {
			hTimeIntervals->SetBinContent(ibin, eventType::beam_4GeV);
			Tbeam2 += hTimeIntervals->GetBinWidth(ibin);
			Qbeam2 += hTimeIntervals->GetBinWidth(ibin) * current; //uC
		}
		/*COSMICS*/
		else if ((current < cosmicsCurrentMax) && (livetime > cosmicsLivetimeMin) && (currentPrev < cosmicsCurrentMax) && (livetimePrev > cosmicsLivetimeMin)) {
			hTimeIntervals->SetBinContent(ibin, eventType::cosmics);
			Tcosmics += hTimeIntervals->GetBinWidth(ibin);
		} else {
			hTimeIntervals->SetBinContent(ibin, 0);
		}
	}
	hTimeIntervals->SetBinContent(0, 0);

	/*Now perform the time-depentent energy correction*/
	hEneVsTime = (TH2D*) myBDXDSTSelector->hEneVsTime->Clone("hEneVsTime");
	hEneCorrection = new TH1D("hEneCorrection", "hEneCorrection", hEneVsTime->GetNbinsX(), 0, Ttot);
	hEneMean = new TH1D("hEneMean", "hEneMean", hEneVsTime->GetNbinsX(), 0, Ttot);

	int fitResult;
	double landauMean;
	TH1D *hProj;
	for (int ibin = 1; ibin < hEneVsTime->GetNbinsX(); ibin++) {

		hProj = hEneVsTime->ProjectionY(Form("proj%i", ibin), ibin, ibin);
		fitResult = hProj->Fit("landau", "L", "", 8, 100);
		hEneCorrection->SetBinContent(ibin, 0);
		if (fitResult == 0) {
			landauMean = hProj->GetFunction("landau")->GetParameter(1);
			if ((landauMean > landauMeanMin) && (landauMean < landauMeanMax)) {
				hEneMean->SetBinContent(ibin, landauMean);
				hEneCorrection->SetBinContent(ibin, landauMeanNominal / landauMean);
			}
		}
	}
	/*Now tag the time of cosmics*/
	double Tcounter=0;
	int TbinID=0;
	hTimeBinID->SetBinContent(0,-1);
	for (int ibin = 1; ibin < hTimeIntervals->GetNbinsX(); ibin++) {
		if (hTimeIntervals->GetBinContent(ibin) == eventType::cosmics) {
			hTimeBinID->SetBinContent(ibin,TbinID);
			Tcounter+=hTimeIntervals->GetBinWidth(ibin);
			/*new bin*/
			if (Tcounter/cosmicsTimeWidth>=1.){
				Tcounter=0;
				TbinID++;
			}
		}
	}

	/*Now, process again*/
	myBDXDSTSelector2->setTimeInterval(Ttot);
	myBDXDSTSelector2->setT0(1. * Tfirst);
	myBDXDSTSelector2->setTimeBin(timeBin);
	myBDXDSTSelector2->setNProof(nproof);
	myBDXDSTSelector2->nEventsTotal = Ntot;
	myBDXDSTSelector2->sethTimeIntervals(hTimeIntervals);
	myBDXDSTSelector2->sethEnergyCorrection(hEneCorrection);

	myBDXDSTSelector2->setPeakMin(TPeakMin);
	myBDXDSTSelector2->setPeakMax(TPeakMax);
	myBDXDSTSelector2->setScintThr(QScintThr);
	DSTChain->Process(myBDXDSTSelector2, opt.c_str(), Ntot, N0);

	/*Draw*/
	TCanvas *c1 = new TCanvas("c1", "First pass");
	c1->Divide(4, 3);
	c1->cd(1);
	myBDXDSTSelector->hTrigAllEvents->Draw("HIST");
	c1->cd(2);
	myBDXDSTSelector->hCur1->SetLineColor(1);
	myBDXDSTSelector->hCur1->Draw("HIST");
	hTimeIntervals->SetLineColor(2);
	hTimeIntervals->Draw("HISTSAME");
	c1->cd(3);
	myBDXDSTSelector->hLive1->SetLineColor(1);
	myBDXDSTSelector->hLive1->Draw("HIST");

	c1->cd(4);
	myBDXDSTSelector->hTrigAllFPGA1->SetLineColor(1);
	myBDXDSTSelector->hTrigAllFPGA1->Draw("HIST");

	myBDXDSTSelector->hTrigAccFPGA1->SetLineColor(2);
	myBDXDSTSelector->hTrigAccFPGA1->Draw("HISTSAME");

	c1->cd(5);
	myBDXDSTSelector->hEne1->SetLineColor(1);
	myBDXDSTSelector->hEne1->Draw("HIST");

	c1->cd(6);
	myBDXDSTSelector->hTemperature1->SetLineColor(1);
	myBDXDSTSelector->hTemperature1->Draw("HIST");

	c1->cd(7);
	myBDXDSTSelector->hEneVsTime->Draw("COLZ");
	c1->cd(8);
	hEneMean->Draw();
	c1->cd(9);
	myBDXDSTSelector->hEneVsPeakTime->Draw("colz");
	c1->cd(10);
	hTimeBinID->Draw();
	hTimeIntervals->Draw("SAME");

	TCanvas *c1a = new TCanvas("c1a", "First pass scintillator1");
	c1a->Divide(3, 3);
	c1a->cd(1);
	myBDXDSTSelector->hQVsTime1Scint5->Draw("colz");
	c1a->cd(2);
	myBDXDSTSelector->hQVsPeakTimeScint5->Draw("colz");
	c1a->cd(4);
	myBDXDSTSelector->hQVsTime1Scint6->Draw("colz");
	c1a->cd(5);
	myBDXDSTSelector->hQVsPeakTimeScint6->Draw("colz");

	TCanvas *c2 = new TCanvas("c2", "Second pass");
	c2->Divide(3, 3);
	c2->cd(1);
	myBDXDSTSelector2->hTrigAllEventsBeam->Draw("HIST");

	myBDXDSTSelector2->hTrigAllEventsBeam2->SetLineColor(3);
	myBDXDSTSelector2->hTrigAllEventsBeam2->Draw("HIST");

	myBDXDSTSelector2->hTrigAllEventsCosmics->SetLineColor(2);
	myBDXDSTSelector2->hTrigAllEventsCosmics->Draw("HISTSAME");

	c2->cd(2);

	TH1D *hEneCrystalBeamTrg2Clone = (TH1D*) myBDXDSTSelector2->hEneCrystalBeamTrg2->Clone();
	hEneCrystalBeamTrg2Clone->Scale(1. / Tbeam, "width");

	TH1D *hEneCrystalBeam2Trg2Clone = (TH1D*) myBDXDSTSelector2->hEneCrystalBeam2Trg2->Clone();
	hEneCrystalBeam2Trg2Clone->Scale(1. / Tbeam2, "width");

	TH1D *hEneCrystalCosmicsTrg2Clone = (TH1D*) myBDXDSTSelector2->hEneCrystalCosmicsTrg2->Clone();
	hEneCrystalCosmicsTrg2Clone->Scale(1. / Tcosmics, "width");

	hEneCrystalBeamTrg2Clone->Draw();

	hEneCrystalBeam2Trg2Clone->SetLineColor(3);
	hEneCrystalBeam2Trg2Clone->Draw("SAMES");

	hEneCrystalCosmicsTrg2Clone->SetLineColor(2);
	hEneCrystalCosmicsTrg2Clone->Draw("SAMES");

	c2->cd(3);

	TH1D *hEneCrystalBeamTrg4Clone = (TH1D*) myBDXDSTSelector2->hEneCrystalBeamTrg4->Clone();
	hEneCrystalBeamTrg4Clone->Scale(1. / Tbeam, "width");

	TH1D *hEneCrystalBeam2Trg4Clone = (TH1D*) myBDXDSTSelector2->hEneCrystalBeam2Trg4->Clone();
	hEneCrystalBeam2Trg4Clone->Scale(1. / Tbeam2, "width");

	TH1D *hEneCrystalCosmicsTrg4Clone = (TH1D*) myBDXDSTSelector2->hEneCrystalCosmicsTrg4->Clone();
	hEneCrystalCosmicsTrg4Clone->Scale(1. / Tcosmics, "width");

	hEneCrystalBeamTrg4Clone->Draw();

	hEneCrystalBeam2Trg4Clone->SetLineColor(3);
	hEneCrystalBeam2Trg4Clone->Draw("SAMES");

	hEneCrystalCosmicsTrg4Clone->SetLineColor(2);
	hEneCrystalCosmicsTrg4Clone->Draw("SAMES");

	c2->cd(5);
	TH1D *hEneNoScintCrystalBeamTrg2Clone = (TH1D*) myBDXDSTSelector2->hEneNoScintCrystalBeamTrg2->Clone();
	hEneNoScintCrystalBeamTrg2Clone->Scale(1. / Tbeam, "width");

	TH1D *hEneNoScintCrystalBeam2Trg2Clone = (TH1D*) myBDXDSTSelector2->hEneNoScintCrystalBeam2Trg2->Clone();
	hEneNoScintCrystalBeam2Trg2Clone->Scale(1. / Tbeam2, "width");

	TH1D *hEneNoScintCrystalCosmicsTrg2Clone = (TH1D*) myBDXDSTSelector2->hEneNoScintCrystalCosmicsTrg2->Clone();
	hEneNoScintCrystalCosmicsTrg2Clone->Scale(1. / Tcosmics, "width");

	hEneNoScintCrystalBeamTrg2Clone->Draw();

	hEneNoScintCrystalBeam2Trg2Clone->SetLineColor(3);
	hEneNoScintCrystalBeam2Trg2Clone->Draw("SAMES");

	hEneNoScintCrystalCosmicsTrg2Clone->SetLineColor(2);
	hEneNoScintCrystalCosmicsTrg2Clone->Draw("SAMES");

	c2->cd(7);
	myBDXDSTSelector2->hEneVsPeakTimeTrg2->Draw("colz");
	c2->cd(8);
	myBDXDSTSelector2->hEneCrystalVsQScint5->Draw("colz");
	c2->cd(9);
	myBDXDSTSelector2->hEneCrystalVsQScint6->Draw("colz");

	cout << "TBEAM: " << Tbeam << endl;
	cout << "EOT: " << Qbeam * 1E-6 / 1.6E-19 << endl;

	cout << "TBEAM2: " << Tbeam2 << endl;
	cout << "EOT2: " << Qbeam2 * 1E-6 / 1.6E-19 << endl;

	cout << "TCOSMICS: " << Tcosmics << endl;

	/*Use this ordering to avoid redo computation @ 11 GeV*/
	TVectorD v(5);
	v[0] = Tbeam;
	v[1] = Qbeam * 1E-6 / 1.6E-19;
	v[2] = Tbeam2;
	v[3] = Qbeam2 * 1E-6 / 1.6E-19;
	v[4] = Tcosmics;

	/*Write histograms on the output file*/
	if (ofname != "") {
		TFile *ofile = new TFile(ofname.c_str(), "recreate");

		TListIter iter(myBDXDSTSelector->GetOutputList());
		TObject *obj;

		while (obj = iter.Next()) {
			if (obj->InheritsFrom(TH1::Class())) obj->Write();
		}

		hEneMean->Write();
		hEneCorrection->Write();
		hTimeIntervals->Write();

		myBDXDSTSelector2->hEneCrystalBeamTrg2->Write();
		myBDXDSTSelector2->hEneCrystalBeam2Trg2->Write();
		myBDXDSTSelector2->hEneCrystalCosmicsTrg2->Write();

		myBDXDSTSelector2->hEneCrystalBeamTrg4->Write();
		myBDXDSTSelector2->hEneCrystalBeam2Trg4->Write();
		myBDXDSTSelector2->hEneCrystalCosmicsTrg4->Write();

		myBDXDSTSelector2->hEneVsPeakTimeTrg2->Write();

		myBDXDSTSelector2->hEneNoScintCrystalBeamTrg2->Write();
		myBDXDSTSelector2->hEneNoScintCrystalBeam2Trg2->Write();
		myBDXDSTSelector2->hEneNoScintCrystalCosmicsTrg2->Write();

		myBDXDSTSelector2->hEneCrystalVsQScint5->Write();
		myBDXDSTSelector2->hEneCrystalVsQScint6->Write();

		v.Write("v");
		ofile->Close();
	}
	if (showGUI) {
		gui.Run(1);
	}
}

