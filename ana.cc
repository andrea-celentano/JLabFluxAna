#include "BDXDSTSelector.h"
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

int isMC;
int nproof;
int Ntot;
int N0;

bool showGUI, doTree;

string ofname, fname;
vector<string> fnames;

void parseCommandLine(int argc, char **argv) {
	isMC = 0;
	nproof = 0;
	Ntot = 0;
	N0 = 0;
	ofname = "";

	showGUI = false;

	for (int ii = 0; ii < argc; ii++) {
		string command = string(argv[ii]);
		if (command == "-f") {
			int jj = ii + 1;
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
		} else if (command == "-MC") {
			isMC = 1;
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

	if (isMC) {
		Info("main", "MC-mode");
	}

	int doFit = 0;

	string opt = "";

	if (isMC) opt = "MC";
	if (doTree) opt = "Tree:" + opt;

	BDXDSTSelector *myBDXDSTSelector = new BDXDSTSelector();
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
	/*Determine last event time*/
	DSTChain->GetEntry(Ntot - 1);
	Tlast = event_tmp->getEventHeader()->getEventTime();

	cout << "From data: first event time is: " << Tfirst << " last event time is: " << Tlast << endl;
	Ttot = Tlast - Tfirst;
	cout << "setting time interval from data: " << Ttot << endl;
	myBDXDSTSelector->setTimeInterval(Ttot);
	cout << "setting first event time: " << Tfirst << endl;
	myBDXDSTSelector->setT0(1. * Tfirst);
	myBDXDSTSelector->setTimeBin(10); //5 minutes per bin
	myBDXDSTSelector->setNProof(nproof);

	if (nproof != 0) {
		cout << "USING PROOF WITH " << nproof << " WORKERS! " << endl;
		TProof::AddEnvVar("PROOF_INITCMD", "$JLAB_ROOT/2.0/ce/jlab.sh");
		TProof *proof = TProof::Open(Form("workers=%i", nproof));

		proof->Exec("gSystem->Load(\"libRooFit.so\")");
		proof->Exec("gSystem->Load(\"${BDXRECO_ROOT}/lib/libJANA.dylib\")");
		proof->Exec("gSystem->Load(\"libbdxRecoExt.dylib\")");
		proof->Exec("gSystem->Load(\"libbdxReco.dylib\")");
		proof->Exec("gSystem->Load(\"libBDXDSTSelector.so\")");
		proof->SetParameter("PROOF_Packetizer", "TPacketizer");
		DSTChain->SetProof(1);
	}

	myBDXDSTSelector->nEventsTotal = Ntot;
	DSTChain->Process(myBDXDSTSelector, opt.c_str(), Ntot, N0);

	cout << "After process " << endl;

	if (isMC == 0) {
		TCanvas *c1 = new TCanvas("c1", "Stability trigger rate");
		c1->Divide(2, 3);
		c1->cd(1);
		myBDXDSTSelector->hTrigAllEvents->Draw("HIST");
		c1->cd(2);
		myBDXDSTSelector->hCur1->SetLineColor(1);
		myBDXDSTSelector->hCur1->Draw("HIST");

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
		myBDXDSTSelector->hEneVsTime->Draw("COLZ");

	}

	/*Write histograms on the output file*/
	if (ofname != "") {
		TFile *ofile = new TFile(ofname.c_str(), "recreate");

		TListIter iter(myBDXDSTSelector->GetOutputList());
		TObject *obj;

		while (obj = iter.Next()) {
			if (obj->InheritsFrom(TH1::Class())) obj->Write();
		}

		ofile->Close();
	}
	if (showGUI) {
		gui.Run(1);
	}
}
