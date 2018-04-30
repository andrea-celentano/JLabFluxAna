void ana() {

	const int nFiles = 9;
	string fnames[nFiles];

	string folder = "data/120s/";

	fnames[0] = "out.627.root";
	fnames[1] = "out.628.root";
	fnames[2] = "out.629.root";
	fnames[3] = "out.633.root";
	fnames[4] = "out.635.root";
	fnames[5] = "out.636.root";
	fnames[6] = "out.638.root";
	fnames[7] = "out.639.root";
	fnames[8] = "out.640.root";

	/**/
	TFile *f;
	TVectorD *v;

	double Tbeam = 0;
	double Tbeam2 = 0;
	double Tcosmics = 0;
	double EOT = 0;
	double EOT2 = 0;

	double eneThr = 1000;

	TH1D *hBeam;
	TH1D *hBeam2;
	TH1D *hCosmics;
	TH1D *hCosmics2;

	TH1D *hBeamNoScint;
	TH1D *hBeam2NoScint;
	TH1D *hCosmicsNoScint;
	TH1D *hCosmics2NoScint;

	TH1D *hBeamTmp;
	TH1D *hBeam2Tmp;
	TH1D *hCosmicsTmp;

	TH1D *hCosmicsAvg;
	TH1D *hCosmicsAvgSum;
	TH1D *hCosmicsSigma;
	TH1D *hCosmicsSigmaSum;

	map<int, vector<TH1D*> > mCosmicsAll;
	map<int, vector<TH1D*> > mCosmicsAllx2;
	map<int, vector<TH1D*> > mCosmicsAllx3;
	map<int, vector<TH1D*> > mCosmicsAllx4;
	vector<TH1D*> vCosmics;
//	vector<TH1D*> *hCosmicsAll=new vector<TH1D*>();
	int nCosmics = 0;

	for (int ii = 0; ii < nFiles; ii++) {
		f = new TFile((folder + fnames[ii]).c_str());
		f->ls();
		v = (TVectorD*) f->Get("v");
		if (v->GetNrows() == 3) {
			Tbeam += (*v)[0];
			EOT += (*v)[1];
			Tcosmics += (*v)[2];
			nCosmics = 0;
		} else if (v->GetNrows() == 5) {
			Tbeam += (*v)[0];
			EOT += (*v)[1];
			Tbeam2 += (*v)[2];
			EOT2 += (*v)[3];
			Tcosmics += (*v)[4];
			nCosmics = 0;
		} else if (v->GetNrows() == 6) {
			Tbeam += (*v)[0];
			EOT += (*v)[1];
			Tbeam2 += (*v)[2];
			EOT2 += (*v)[3];
			Tcosmics += (*v)[4];
			nCosmics = (int) ((*v)[5]);
		}
		if (ii == 0) {
			hBeam = (TH1D*) f->Get("hEneCrystalBeamTrg2");
			hBeam2 = (TH1D*) f->Get("hEneCrystalBeam2Trg2");
			hCosmics = (TH1D*) f->Get("hEneCrystalCosmicsTrg2");

			hBeamNoScint = (TH1D*) f->Get("hEneNoScintCrystalBeamTrg2");
			hBeam2NoScint = (TH1D*) f->Get("hEneNoScintCrystalBeam2Trg2");
			hCosmicsNoScint = (TH1D*) f->Get("hEneNoScintCrystalCosmicsTrg2");

		} else {
			hBeamTmp = (TH1D*) f->Get("hEneCrystalBeamTrg2");
			hBeam2Tmp = (TH1D*) f->Get("hEneCrystalBeam2Trg2");
			hCosmicsTmp = (TH1D*) f->Get("hEneCrystalCosmicsTrg2");

			hBeam->Add(hBeamTmp);
			if (hBeam2Tmp != 0) hBeam2->Add(hBeam2Tmp);
			hCosmics->Add(hCosmicsTmp);

			hBeamTmp = (TH1D*) f->Get("hEneNoScintCrystalBeamTrg2");
			hBeam2Tmp = (TH1D*) f->Get("hEneNoScintCrystalBeam2Trg2");
			hCosmicsTmp = (TH1D*) f->Get("hEneNoScintCrystalCosmicsTrg2");

			hBeamNoScint->Add(hBeamTmp);
			if (hBeam2Tmp != 0) hBeam2NoScint->Add(hBeam2Tmp);
			hCosmicsNoScint->Add(hCosmicsTmp);

		}
		/*Ignore last histo, since it is less than the time width of the others*/
		if (nCosmics > 1) {
			nCosmics--;
			vCosmics.clear();
			for (int jj = 0; jj < nCosmics; jj++) {
				vCosmics.push_back((TH1D*) f->Get(Form("hEneCrystalCosmicsTrg2_%i", jj)));
			}
			mCosmicsAll[ii] = vCosmics;

			/*Add 2 by 2 histos to check x2*/
			vCosmics.clear();
			for (int jj = 0; jj < nCosmics / 2; jj++) {
				vCosmics.push_back((TH1D*) f->Get(Form("hEneCrystalCosmicsTrg2_%i", jj * 2))->Clone(Form("hEneCrystalCosmicsTrg2x2_%i", jj * 2)));
				vCosmics[jj]->Add((TH1D*) f->Get(Form("hEneCrystalCosmicsTrg2_%i", jj * 2 + 1)));
			}
			mCosmicsAllx2[ii] = vCosmics;

			/*Add 3 by 3 histos to check x3*/
			vCosmics.clear();
			for (int jj = 0; jj < nCosmics / 3; jj++) {
				vCosmics.push_back((TH1D*) f->Get(Form("hEneCrystalCosmicsTrg2_%i", jj * 3))->Clone(Form("hEneCrystalCosmicsTrg2x3_%i", jj * 3)));
				vCosmics[jj]->Add((TH1D*) f->Get(Form("hEneCrystalCosmicsTrg2_%i", jj * 3 + 1)));
				vCosmics[jj]->Add((TH1D*) f->Get(Form("hEneCrystalCosmicsTrg2_%i", jj * 3 + 2)));
			}
			mCosmicsAllx3[ii] = vCosmics;

			/*Add 3 by 3 histos to check x4*/
			vCosmics.clear();
			for (int jj = 0; jj < nCosmics / 4; jj++) {
				vCosmics.push_back((TH1D*) f->Get(Form("hEneCrystalCosmicsTrg2_%i", jj * 4))->Clone(Form("hEneCrystalCosmicsTrg2x4_%i", jj * 4)));
				vCosmics[jj]->Add((TH1D*) f->Get(Form("hEneCrystalCosmicsTrg2_%i", jj * 4 + 1)));
				vCosmics[jj]->Add((TH1D*) f->Get(Form("hEneCrystalCosmicsTrg2_%i", jj * 4 + 2)));
				vCosmics[jj]->Add((TH1D*) f->Get(Form("hEneCrystalCosmicsTrg2_%i", jj * 4 + 3)));
			}
			mCosmicsAllx4[ii] = vCosmics;

		}

	}
	hCosmics2 = (TH1D*) hCosmics->Clone("hCosmics2");
	hCosmics2NoScint = (TH1D*) hCosmicsNoScint->Clone("hCosmics2NoScint");

	hCosmics->Scale(Tbeam / Tcosmics);
	hCosmics2->Scale(Tbeam2 / Tcosmics);

	hCosmicsNoScint->Scale(Tbeam / Tcosmics);
	hCosmics2NoScint->Scale(Tbeam2 / Tcosmics);

	int bin1, bin2;
	bin1 = hBeam->FindBin(eneThr);
	bin2 = hBeam->FindBin(1990.);

	double integralCosmics, integralBeam;
	double integralCosmicsNoScint, integralBeamNoScint;

	integralCosmics = hCosmics->Integral(bin1, bin2);
	integralBeam = hBeam->Integral(bin1, bin2);
	integralCosmicsNoScint = hCosmicsNoScint->Integral(bin1, bin2);
	integralBeamNoScint = hBeamNoScint->Integral(bin1, bin2);
	cout << "11GeV: " << endl;
	cout << "Tbeam: " << Tbeam << " EOT: " << EOT << " TCosmics: " << Tcosmics <<" Tratio: "<<Tcosmics/Tbeam2<< endl;
	cout << eneThr << "-MeV: " << integralBeam << " " << integralCosmics <<" not-scaled: "<<integralCosmics*Tcosmics/Tbeam2<< endl;
	cout << eneThr << "-MeV noScint: " << integralBeamNoScint << " " << integralCosmicsNoScint << endl;

	double integralCosmics2, integralBeam2;
	double integralCosmics2NoScint, integralBeam2NoScint;
	bin1 = hBeam2->FindBin(eneThr);
	bin2 = hBeam2->FindBin(1990.);
	integralCosmics2 = hCosmics2->Integral(bin1, bin2);
	integralBeam2 = hBeam2->Integral(bin1, bin2);
	integralCosmics2NoScint = hCosmics2NoScint->Integral(bin1, bin2);
	integralBeam2NoScint = hBeam2NoScint->Integral(bin1, bin2);
	cout << "4GeV: " << endl;
	cout << "Tbeam: " << Tbeam2 << " EOT: " << EOT2 << " TCosmics: " << Tcosmics <<" Tratio: "<<Tcosmics/Tbeam2<<endl;
	cout << eneThr << "-MeV: " << integralBeam2 << " " << integralCosmics2 <<" not-scaled: "<<integralCosmics2*Tcosmics/Tbeam2<< endl;
	cout << eneThr << "-MeV noScint: " << integralBeam2NoScint << " " << integralCosmics2NoScint << endl;

	/*Cosmics*/

	TGraph *grInt1 = new TGraph();
	TGraph *grInt2 = new TGraph();
	TGraph *grInt3 = new TGraph();
	TGraph *grInt4 = new TGraph();

	TGraph *grInt1x2 = new TGraph();
	TGraph *grInt2x2 = new TGraph();
	TGraph *grInt3x2 = new TGraph();
	TGraph *grInt4x2 = new TGraph();

	TGraph *grInt1x3 = new TGraph();
	TGraph *grInt2x3 = new TGraph();
	TGraph *grInt3x3 = new TGraph();
	TGraph *grInt4x3 = new TGraph();

	TGraph *grInt1x4 = new TGraph();
	TGraph *grInt2x4 = new TGraph();
	TGraph *grInt3x4 = new TGraph();
	TGraph *grInt4x4 = new TGraph();

	TH1D *hInt1 = new TH1D("hInt1", "hInt1", 2001, -0.5, 2000.5);
	TH1D *hInt2 = new TH1D("hInt2", "hInt2", 2001, -0.5, 2000.5);
	TH1D *hInt3 = new TH1D("hInt3", "hInt3", 2001, -0.5, 2000.5);
	TH1D *hInt4 = new TH1D("hInt4", "hInt4", 2001, -0.5, 2000.5);

	TH1D *hInt1x2 = new TH1D("hInt1x2", "hInt1x2", 2001, -0.5, 2000.5);
	TH1D *hInt2x2 = new TH1D("hInt2x2", "hInt2x2", 2001, -0.5, 2000.5);
	TH1D *hInt3x2 = new TH1D("hInt3x2", "hInt3x2", 2001, -0.5, 2000.5);
	TH1D *hInt4x2 = new TH1D("hInt4x2", "hInt4x2", 2001, -0.5, 2000.5);

	TH1D *hInt1x3 = new TH1D("hInt1x3", "hInt1x3", 2001, -0.5, 2000.5);
	TH1D *hInt2x3 = new TH1D("hInt2x3", "hInt2x3", 2001, -0.5, 2000.5);
	TH1D *hInt3x3 = new TH1D("hInt3x3", "hInt3x3", 2001, -0.5, 2000.5);
	TH1D *hInt4x3 = new TH1D("hInt4x3", "hInt4x3", 2001, -0.5, 2000.5);

	TH1D *hInt1x4 = new TH1D("hInt1x4", "hInt1x4", 2001, -0.5, 2000.5);
	TH1D *hInt2x4 = new TH1D("hInt2x4", "hInt2x4", 2001, -0.5, 2000.5);
	TH1D *hInt3x4 = new TH1D("hInt3x4", "hInt3x4", 2001, -0.5, 2000.5);
	TH1D *hInt4x4 = new TH1D("hInt4x4", "hInt4x4", 2001, -0.5, 2000.5);

	double E1 = 100.;
	double E2 = 200.;
	double E3 = 300.;
	double E4 = 400.;

	int ip = 0;
	int ipx2 = 0;
	int ipx3 = 0;
	int ipx4 = 0;

	for (int ii = 0; ii < mCosmicsAll.size(); ii++) { //ii is the file number
		vCosmics = mCosmicsAll[ii];
		for (int jj = 0; jj < vCosmics.size(); jj++) {
			bin1 = vCosmics[jj]->FindBin(E1);
			bin2 = vCosmics[jj]->FindBin(1990.);
			integralCosmics = vCosmics[jj]->Integral(bin1, bin2);
			grInt1->SetPoint(ip, 100 * ii + jj, integralCosmics);
			hInt1->Fill(integralCosmics);

			bin1 = vCosmics[jj]->FindBin(E2);
			bin2 = vCosmics[jj]->FindBin(1990.);
			integralCosmics = vCosmics[jj]->Integral(bin1, bin2);
			grInt2->SetPoint(ip, 100 * ii + jj, integralCosmics);
			hInt2->Fill(integralCosmics);

			bin1 = vCosmics[jj]->FindBin(E3);
			bin2 = vCosmics[jj]->FindBin(1990.);
			integralCosmics = vCosmics[jj]->Integral(bin1, bin2);
			grInt3->SetPoint(ip, 100 * ii + jj, integralCosmics);
			hInt3->Fill(integralCosmics);

			bin1 = vCosmics[jj]->FindBin(E4);
			bin2 = vCosmics[jj]->FindBin(1990.);
			integralCosmics = vCosmics[jj]->Integral(bin1, bin2);
			grInt4->SetPoint(ip, 100 * ii + jj, integralCosmics);
			hInt4->Fill(integralCosmics);

			ip++;
		}
		vCosmics = mCosmicsAllx2[ii];
		for (int jj = 0; jj < vCosmics.size(); jj++) {
			bin1 = vCosmics[jj]->FindBin(E1);
			bin2 = vCosmics[jj]->FindBin(1990.);
			integralCosmics = vCosmics[jj]->Integral(bin1, bin2);
			grInt1x2->SetPoint(ipx2, 100 * ii + jj * 2, integralCosmics / 2);
			hInt1x2->Fill(integralCosmics);

			bin1 = vCosmics[jj]->FindBin(E2);
			bin2 = vCosmics[jj]->FindBin(1990.);
			integralCosmics = vCosmics[jj]->Integral(bin1, bin2);
			grInt2x2->SetPoint(ipx2, 100 * ii + jj * 2, integralCosmics / 2);
			hInt2x2->Fill(integralCosmics);

			bin1 = vCosmics[jj]->FindBin(E3);
			bin2 = vCosmics[jj]->FindBin(1990.);
			integralCosmics = vCosmics[jj]->Integral(bin1, bin2);
			grInt3x2->SetPoint(ipx2, 100 * ii + jj * 2, integralCosmics / 2);
			hInt3x2->Fill(integralCosmics);

			bin1 = vCosmics[jj]->FindBin(E4);
			bin2 = vCosmics[jj]->FindBin(1990.);
			integralCosmics = vCosmics[jj]->Integral(bin1, bin2);
			grInt4x2->SetPoint(ipx2, 100 * ii + jj * 2, integralCosmics / 2);
			hInt4x2->Fill(integralCosmics);

			ipx2++;
		}

		vCosmics = mCosmicsAllx3[ii];
		for (int jj = 0; jj < vCosmics.size(); jj++) {
			bin1 = vCosmics[jj]->FindBin(E1);
			bin2 = vCosmics[jj]->FindBin(1990.);
			integralCosmics = vCosmics[jj]->Integral(bin1, bin2);
			grInt1x3->SetPoint(ipx3, 100 * ii + jj * 3, integralCosmics / 3);
			hInt1x3->Fill(integralCosmics);

			bin1 = vCosmics[jj]->FindBin(E2);
			bin2 = vCosmics[jj]->FindBin(1990.);
			integralCosmics = vCosmics[jj]->Integral(bin1, bin2);
			grInt2x3->SetPoint(ipx3, 100 * ii + jj * 3, integralCosmics / 3);
			hInt2x3->Fill(integralCosmics);

			bin1 = vCosmics[jj]->FindBin(E3);
			bin2 = vCosmics[jj]->FindBin(1990.);
			integralCosmics = vCosmics[jj]->Integral(bin1, bin2);
			grInt3x3->SetPoint(ipx3, 100 * ii + jj * 3, integralCosmics / 3);
			hInt3x3->Fill(integralCosmics);

			bin1 = vCosmics[jj]->FindBin(E4);
			bin2 = vCosmics[jj]->FindBin(1990.);
			integralCosmics = vCosmics[jj]->Integral(bin1, bin2);
			grInt4x3->SetPoint(ipx3, 100 * ii + jj * 3, integralCosmics / 3);
			hInt4x3->Fill(integralCosmics);

			ipx3++;
		}

		vCosmics = mCosmicsAllx4[ii];
		for (int jj = 0; jj < vCosmics.size(); jj++) {
			bin1 = vCosmics[jj]->FindBin(E1);
			bin2 = vCosmics[jj]->FindBin(1990.);
			integralCosmics = vCosmics[jj]->Integral(bin1, bin2);
			grInt1x4->SetPoint(ipx4, 100 * ii + jj, integralCosmics / 4);
			hInt1x4->Fill(integralCosmics);

			bin1 = vCosmics[jj]->FindBin(E2);
			bin2 = vCosmics[jj]->FindBin(1990.);
			integralCosmics = vCosmics[jj]->Integral(bin1, bin2);
			grInt2x4->SetPoint(ipx4, 100 * ii + jj * 4, integralCosmics / 4);
			hInt2x4->Fill(integralCosmics);

			bin1 = vCosmics[jj]->FindBin(E3);
			bin2 = vCosmics[jj]->FindBin(1990.);
			integralCosmics = vCosmics[jj]->Integral(bin1, bin2);
			grInt3x4->SetPoint(ipx4, 100 * ii + jj * 4, integralCosmics / 4);
			hInt3x4->Fill(integralCosmics);

			bin1 = vCosmics[jj]->FindBin(E4);
			bin2 = vCosmics[jj]->FindBin(1990.);
			integralCosmics = vCosmics[jj]->Integral(bin1, bin2);
			grInt4x4->SetPoint(ipx4, 100 * ii + jj * 4, integralCosmics / 4);
			hInt4x4->Fill(integralCosmics);

			ipx4++;
		}

	}

	/*Avg histo*/
	hCosmicsAvg = (TH1D*) (hCosmics->Clone("hCosmicsAvg"));
	hCosmicsAvg->Reset();
	hCosmicsSigma = (TH1D*) (hCosmics->Clone("hCosmicsSigma"));
	hCosmicsSigma->Reset();
	hCosmicsAvgSum = (TH1D*) (hCosmics->Clone("hCosmicsAvgSum"));
	hCosmicsAvgSum->Reset();
	hCosmicsSigmaSum = (TH1D*) (hCosmics->Clone("hCosmicsSigmaSum"));
	hCosmicsSigmaSum->Reset();

	vector<double> data;
	vector<double> dataSum;

	for (int ibin = 0; ibin <= hCosmicsAvg->GetNbinsX(); ibin++) {
		data.clear();
		dataSum.clear();
		for (int ii = 0; ii < mCosmicsAll.size(); ii++) { //ii is the file number
			vCosmics = mCosmicsAll[ii];
			for (int jj = 0; jj < vCosmics.size(); jj++) {
				data.push_back(vCosmics[jj]->GetBinContent(ibin));
				dataSum.push_back(vCosmics[jj]->Integral(ibin, vCosmics[jj]->FindBin(1990)));
			}
		}

		hCosmicsAvg->SetBinContent(ibin, TMath::Mean(data.begin(), data.end()));
		hCosmicsAvg->SetBinError(ibin, TMath::RMS(data.begin(), data.end()));

		hCosmicsAvgSum->SetBinContent(ibin, TMath::Mean(dataSum.begin(), dataSum.end()));
		hCosmicsAvgSum->SetBinError(ibin, TMath::RMS(dataSum.begin(), dataSum.end()));

		hCosmicsSigma->SetBinContent(ibin, TMath::RMS(data.begin(), data.end()) / sqrt(TMath::Mean(data.begin(), data.end()) + 0.01));
		hCosmicsSigmaSum->SetBinContent(ibin, TMath::RMS(dataSum.begin(), dataSum.end()) / sqrt(TMath::Mean(dataSum.begin(), dataSum.end()) + 0.01));
	}

	cout << E1 << " " << hInt1->GetMean() << " " << hInt1->GetRMS() << " (" << sqrt(hInt1->GetMean()) << ")" << endl;
	cout << E2 << " " << hInt2->GetMean() << " " << hInt2->GetRMS() << " (" << sqrt(hInt2->GetMean()) << ")" << endl;
	cout << E3 << " " << hInt3->GetMean() << " " << hInt3->GetRMS() << " (" << sqrt(hInt3->GetMean()) << ")" << endl;
	cout << E4 << " " << hInt4->GetMean() << " " << hInt4->GetRMS() << " (" << sqrt(hInt4->GetMean()) << ")" << endl;

	TGraph *gSum = new TGraph();
	gSum->SetPoint(0, E1, hInt1->GetRMS());
	gSum->SetPoint(1, E2, hInt2->GetRMS());
	gSum->SetPoint(2, E3, hInt3->GetRMS());
	gSum->SetPoint(3, E4, hInt4->GetRMS());

	TGraph *gSumx2 = new TGraph();
	gSumx2->SetPoint(0, E1, hInt1x2->GetRMS() / sqrt(2.));
	gSumx2->SetPoint(1, E2, hInt2x2->GetRMS() / sqrt(2.));
	gSumx2->SetPoint(2, E3, hInt3x2->GetRMS() / sqrt(2.));
	gSumx2->SetPoint(3, E4, hInt4x2->GetRMS() / sqrt(2.));

	TGraph *gSumx3 = new TGraph();
	gSumx3->SetPoint(0, E1, hInt1x3->GetRMS() / sqrt(3.));
	gSumx3->SetPoint(1, E2, hInt2x3->GetRMS() / sqrt(3.));
	gSumx3->SetPoint(2, E3, hInt3x3->GetRMS() / sqrt(3.));
	gSumx3->SetPoint(3, E4, hInt4x3->GetRMS() / sqrt(3.));

	TGraph *gSumx4 = new TGraph();
	gSumx4->SetPoint(0, E1, hInt1x4->GetRMS() / sqrt(4.));
	gSumx4->SetPoint(1, E2, hInt2x4->GetRMS() / sqrt(4.));
	gSumx4->SetPoint(2, E3, hInt3x4->GetRMS() / sqrt(4.));
	gSumx4->SetPoint(3, E4, hInt4x4->GetRMS() / sqrt(4.));

	TCanvas *c = new TCanvas("c", "c");
	c->Divide(2, 2);
	c->cd(1);
	hBeam->Draw();
	hCosmics->SetLineColor(2);
	hCosmics->Draw("HISTSAMESS");

	c->cd(2);
	hBeam2->Draw();
	hCosmics2->SetLineColor(2);
	hCosmics2->Draw("HISTSAMESS");

	c->cd(3);
	hBeamNoScint->Draw();
	hCosmicsNoScint->SetLineColor(2);
	hCosmicsNoScint->Draw("HISTSAMESS");

	c->cd(4);
	hBeam2NoScint->Draw();
	hCosmics2NoScint->SetLineColor(2);
	hCosmics2NoScint->Draw("HISTSAMESS");

	TCanvas *c2 = new TCanvas("c2", "c2");
	c2->Divide(4, 3);
	c2->cd(1);
	grInt1->SetMarkerStyle(20);
	grInt1->Draw("AP");
	grInt1x2->SetMarkerStyle(20);
	grInt1x2->SetMarkerColor(2);
	grInt1x2->Draw("PSAMES");
	grInt1x3->SetMarkerStyle(20);
	grInt1x3->SetMarkerColor(3);
	grInt1x3->Draw("PSAMES");

	c2->cd(2);
	grInt2->SetMarkerStyle(20);
	grInt2->Draw("AP");
	grInt2x2->SetMarkerStyle(20);
	grInt2x2->SetMarkerColor(2);
	grInt2x2->Draw("PSAMES");
	grInt2x3->SetMarkerStyle(20);
	grInt2x3->SetMarkerColor(3);
	grInt2x3->Draw("PSAMES");

	c2->cd(3);
	grInt3->SetMarkerStyle(20);
	grInt3->Draw("AP");
	grInt3x2->SetMarkerStyle(20);
	grInt3x2->SetMarkerColor(2);
	grInt3x2->Draw("PSAMES");
	grInt3x3->SetMarkerStyle(20);
	grInt3x3->SetMarkerColor(3);
	grInt3x3->Draw("PSAMES");

	c2->cd(4);
	grInt4->SetMarkerStyle(20);
	grInt4->Draw("AP");
	grInt4x2->SetMarkerStyle(20);
	grInt4x2->SetMarkerColor(2);
	grInt4x2->Draw("PSAMES");
	grInt4x3->SetMarkerStyle(20);
	grInt4x3->SetMarkerColor(3);
	grInt4x3->Draw("PSAMES");

	c2->cd(5);
	hInt1->Draw();
	hInt1x2->SetLineColor(2);
	hInt1x2->Draw("SAMES");
	hInt1x3->SetLineColor(3);
	hInt1x3->Draw("SAMES");

	c2->cd(6);
	hInt2->Draw();
	hInt2x2->SetLineColor(2);
	hInt2x2->Draw("SAMES");
	hInt2x3->SetLineColor(3);
	hInt2x3->Draw("SAMES");

	c2->cd(7);
	hInt3->Draw();
	hInt3x2->SetLineColor(2);
	hInt3x2->Draw("SAMES");
	hInt3x3->SetLineColor(3);
	hInt3x3->Draw("SAMES");

	c2->cd(8);
	hInt4->Draw();
	hInt4x2->SetLineColor(2);
	hInt4x2->Draw("SAMES");
	hInt4x3->SetLineColor(3);
	hInt4x3->Draw("SAMES");

	c2->cd(9);
	gSum->SetMarkerStyle(20);
	gSum->Draw("AP");
	gSumx2->SetMarkerStyle(20);
	gSumx2->SetMarkerColor(2);
	gSumx2->Draw("PSAMES");
	gSumx3->SetMarkerStyle(20);
	gSumx3->SetMarkerColor(3);
	gSumx3->Draw("PSAMES");

	c2->cd(10);
	hCosmicsAvg->Draw();
	hCosmicsAvgSum->SetLineColor(2);
	hCosmicsAvgSum->Draw("SAMES");
	c2->cd(11);
	hCosmicsSigma->Draw();
	hCosmicsSigmaSum->SetLineColor(2);
	hCosmicsSigmaSum->Draw("SAMES");

	TCanvas *ctmp=new TCanvas("ctmp","ctmp");
	ctmp->Divide(2,1);
	ctmp->cd(1);
	hCosmicsAvg->Draw();
	hCosmicsAvgSum->SetLineColor(2);
	hCosmicsAvgSum->Draw("SAMES");
	ctmp->cd(2);
	hCosmicsSigma->Draw();
	hCosmicsSigmaSum->SetLineColor(2);
	hCosmicsSigmaSum->Draw("SAMES");


}
