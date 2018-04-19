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
		} else if (command == "-GUI") showGUI = true;
		else if (command == "-tree") doTree = true;
	}
}

int main(int argc, char **argv) {

	TApplication gui("GUI", 0, NULL);
	parseCommandLine(argc, argv);

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

	double timeBin=30;

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

	const double beamCurrentMin = 8;
	const double beamEnergyMin = 10E3; //MeV
	const double beamLivetimeMin = 90;

	const double cosmicsCurrentMax = .1;
	const double cosmicsLivetimeMin = 90;

	const double landauMeanMin = 10;
	const double landauMeanMax = 40;

	const double landauMeanNominal = 30;

	double Tbeam = 0;
	double Qbeam = 0;
	double Tcosmics = 0;

	hTimeIntervals = (TH1D*) myBDXDSTSelector->hCur1->Clone("hTimeIntervals");
	hTimeIntervals->SetTitle("hTimeIntervals");
	hTimeIntervals->Reset();

	cout << hTimeIntervals->GetNbinsX() << endl;

	/*In order to be conservative, the time-intervals analysis proceeds in this way:
	 * 1. I loop over all the bins in hCur1, hLive1, hEne1 histograms
	 * 2. For a given bin to be considered "cosmics" or "beam", that beam AND THE PREVIOUS ONE must satisfy the requirements
	 * 3. The most efficient way to proceed is thus to loop from the maximum bin to the minimum one
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

		/*BEAM*/
		if ((current > beamCurrentMin) && (energy > beamEnergyMin) && (livetime > beamLivetimeMin) && (currentPrev > beamCurrentMin) && (energyPrev > beamEnergyMin) && (livetimePrev > beamLivetimeMin)) {
			hTimeIntervals->SetBinContent(ibin, 1);
			Tbeam += hTimeIntervals->GetBinWidth(ibin);
			Qbeam += hTimeIntervals->GetBinWidth(ibin) * current; //uC
		}
		/*COSMICS*/
		else if ((current < cosmicsCurrentMax) && (livetime > cosmicsLivetimeMin) && (currentPrev < cosmicsCurrentMax) && (livetimePrev > cosmicsLivetimeMin)) {
			hTimeIntervals->SetBinContent(ibin, 2);
			Tcosmics += hTimeIntervals->GetBinWidth(ibin);
		} else {
			hTimeIntervals->SetBinContent(ibin, 0);
		}
	}
	hTimeIntervals->SetBinContent(0, 0);


	/*Now perform the time-depentent energy correction*/
	hEneVsTime=(TH2D*)myBDXDSTSelector->hEneVsTime->Clone("hEneVsTime");
	hEneCorrection=new TH1D("hEneCorrection","hEneCorrection",hEneVsTime->GetNbinsX(),0,Ttot);
	hEneMean=new TH1D("hEneMean","hEneMean",hEneVsTime->GetNbinsX(),0,Ttot);

	int fitResult;
	double landauMean;
	TH1D *hProj;
	for (int ibin = 1; ibin < hEneVsTime->GetNbinsX(); ibin++){

		hProj=hEneVsTime->ProjectionY(Form("proj%i",ibin),ibin,ibin);
		fitResult=hProj->Fit("landau","","",8,100);
		hEneCorrection->SetBinContent(ibin,0);
		if (fitResult==0){
			landauMean=hProj->GetFunction("landau")->GetParameter(1);
			if ((landauMean > landauMeanMin)&&(landauMean < landauMeanMax)){
				hEneMean->SetBinContent(ibin,landauMean);
				hEneCorrection->SetBinContent(ibin,landauMeanNominal/landauMean);
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


	DSTChain->Process(myBDXDSTSelector2, opt.c_str(), Ntot, N0);

	/*Draw*/
	TCanvas *c1 = new TCanvas("c1", "First pass");
	c1->Divide(3, 3);
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

	c1->cd(7);
	myBDXDSTSelector->hEneVsTime->Draw("COLZ");
	c1->cd(8);
	hEneMean->Draw();
	c1->cd(9);
	hEneCorrection->Draw();

	TCanvas *c2 = new TCanvas("c2", "Second pass");
	c2->Divide(2, 3);
	c2->cd(1);
	myBDXDSTSelector2->hTrigAllEventsBeam->Draw("HIST");
	myBDXDSTSelector2->hTrigAllEventsCosmics->SetLineColor(2);
	myBDXDSTSelector2->hTrigAllEventsCosmics->Draw("HISTSAME");

	c2->cd(2);

	TH1D *hEneCrystalBeamClone =(TH1D*) myBDXDSTSelector2->hEneCrystalBeam->Clone();
	hEneCrystalBeamClone->Scale(1. / Tbeam, "width");

	TH1D *hEneCrystalCosmicsClone =(TH1D*) myBDXDSTSelector2->hEneCrystalCosmics->Clone();
	hEneCrystalCosmicsClone->Scale(1. / Tcosmics, "width");


	hEneCrystalBeamClone->Draw();
	hEneCrystalCosmicsClone->SetLineColor(2);
	hEneCrystalCosmicsClone->Draw("SAMES");

	cout << "TBEAM: " << Tbeam << endl;
	cout << "EOT: " << Qbeam * 1E-6 / 1.6E-19 << endl;
	cout << "TCOSMICS: " << Tcosmics << endl;

	TVectorD v(3);
	v[0] = Tbeam;
	v[1] = Qbeam * 1E-6 / 1.6E-19;
	v[2] = Tcosmics;

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

		myBDXDSTSelector2->hEneCrystalBeam->Write();
		myBDXDSTSelector2->hEneCrystalCosmics->Write();

		v.Write("v");
		ofile->Close();
	}
	if (showGUI) {
		gui.Run(1);
	}
}

