void ana(){

  const int nFiles=9;
  string fnames[nFiles];

  fnames[0]="out.627.root";
  fnames[1]="out.628.root";
  fnames[2]="out.629.root";
  fnames[3]="out.633.root";
  fnames[4]="out.635.root";
  fnames[5]="out.636.root";
  fnames[6]="out.638.root";
  fnames[7]="out.639.root";
  fnames[8]="out.640.root";
  /**/
  TFile *f;
  TVectorD *v;

  double Tbeam=0;
  double Tbeam2=0;
  double Tcosmics=0;
  double EOT=0;
  double EOT2=0;

  double eneThr=400;

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

  vector<TH1D*> *hCosmicsAll;
  
  for (int ii=0;ii<nFiles;ii++){
    f=new TFile(fnames[ii].c_str());
    f->ls();
    v=(TVectorD*)f->Get("v");
    if (v->GetNrows()==3){
      Tbeam+=(*v)[0];
      EOT+=(*v)[1];
      Tcosmics+=(*v)[2];
    }
    else if (v->GetNrows()==5){
      Tbeam+=(*v)[0];
      EOT+=(*v)[1];
      Tbeam2+=(*v)[2];
      EOT2+=(*v)[3];
      Tcosmics+=(*v)[4];
    }
    if (ii==0){
      hBeam=(TH1D*)f->Get("hEneCrystalBeamTrg2");
      hBeam2=(TH1D*)f->Get("hEneCrystalBeam2Trg2");
      hCosmics=(TH1D*)f->Get("hEneCrystalCosmicsTrg2");

      hBeamNoScint=(TH1D*)f->Get("hEneNoScintCrystalBeamTrg2");
      hBeam2NoScint=(TH1D*)f->Get("hEneNoScintCrystalBeam2Trg2");
      hCosmicsNoScint=(TH1D*)f->Get("hEneNoScintCrystalCosmicsTrg2");
      
    }
    else{
      hBeamTmp=(TH1D*)f->Get("hEneCrystalBeamTrg2");
      hBeam2Tmp=(TH1D*)f->Get("hEneCrystalBeam2Trg2");
      hCosmicsTmp=(TH1D*)f->Get("hEneCrystalCosmicsTrg2");

      hBeam->Add(hBeamTmp);
      if (hBeam2Tmp!=0) hBeam2->Add(hBeam2Tmp);
      hCosmics->Add(hCosmicsTmp);

      hBeamTmp=(TH1D*)f->Get("hEneNoScintCrystalBeamTrg2");
      hBeam2Tmp=(TH1D*)f->Get("hEneNoScintCrystalBeam2Trg2");
      hCosmicsTmp=(TH1D*)f->Get("hEneNoScintCrystalCosmicsTrg2");

      hBeamNoScint->Add(hBeamTmp);
      if (hBeam2Tmp!=0) hBeam2NoScint->Add(hBeam2Tmp);
      hCosmicsNoScint->Add(hCosmicsTmp);
      
    }
  }
  hCosmics2=(TH1D*)hCosmics->Clone("hCosmics2");
  hCosmics2NoScint=(TH1D*)hCosmicsNoScint->Clone("hCosmics2NoScint");

  hCosmics->Scale(Tbeam/Tcosmics);
  hCosmics2->Scale(Tbeam2/Tcosmics);

  hCosmicsNoScint->Scale(Tbeam/Tcosmics);
  hCosmics2NoScint->Scale(Tbeam2/Tcosmics);
  
  int bin1,bin2;
  bin1=hBeam->FindBin(eneThr);
  bin2=hBeam->FindBin(1990.);

  double integralCosmics,integralBeam;
  double integralCosmicsNoScint,integralBeamNoScint;

  integralCosmics=hCosmics->Integral(bin1,bin2);
  integralBeam=hBeam->Integral(bin1,bin2);
  integralCosmicsNoScint=hCosmicsNoScint->Integral(bin1,bin2);
  integralBeamNoScint=hBeamNoScint->Integral(bin1,bin2);
  cout<<"11GeV: "<<endl;
  cout<<"Tbeam: "<<Tbeam<<" EOT: "<<EOT<<" TCosmics: "<<Tcosmics<<endl;
  cout<<"500-MeV: "<<integralBeam<<" "<<integralCosmics<<endl;
  cout<<"500-MeV noScint: "<<integralBeamNoScint<<" "<<integralCosmicsNoScint<<endl;


  
  double integralCosmics2,integralBeam2;
  double integralCosmics2NoScint,integralBeam2NoScint;
  bin1=hBeam2->FindBin(eneThr);
  bin2=hBeam2->FindBin(1990.); 
  integralCosmics2=hCosmics2->Integral(bin1,bin2);
  integralBeam2=hBeam2->Integral(bin1,bin2);
  integralCosmics2NoScint=hCosmics2NoScint->Integral(bin1,bin2);
  integralBeam2NoScint=hBeam2NoScint->Integral(bin1,bin2);
  cout<<"4GeV: "<<endl;
  cout<<"Tbeam: "<<Tbeam2<<" EOT: "<<EOT2<<" TCosmics: "<<Tcosmics<<endl;
  cout<<"500-MeV: "<<integralBeam2<<" "<<integralCosmics<<endl;
  cout<<"500-MeV noScint: "<<integralBeam2NoScint<<" "<<integralCosmics2NoScint<<endl;

					    
  TCanvas *c=new TCanvas("c","c");
  c->Divide(2,2);
  c->cd(1);
  hBeam->Draw();
  hCosmics->SetLineColor(2);
  hCosmics->Draw("HISTSAMES");

  c->cd(2);
  hBeam2->Draw();
  hCosmics2->SetLineColor(2);
  hCosmics2->Draw("HISTSAMES");

  c->cd(3);
  hBeamNoScint->Draw();
  hCosmicsNoScint->SetLineColor(2);
  hCosmicsNoScint->Draw("HISTSAMES");

  c->cd(4);
  hBeam2NoScint->Draw();
  hCosmics2NoScint->SetLineColor(2);
  hCosmics2NoScint->Draw("HISTSAMES");

  
  
}
