void ana(){

  const int nFiles=7;
  string fnames[nFiles];

  fnames[0]="out.627.root";
  fnames[1]="out.628.root";
  fnames[2]="out.629.root";
  fnames[3]="out.633.root";
  fnames[4]="out.635.root";
  fnames[5]="out.636.root";
  fnames[6]="out.638.root";

  TFile *f;
  TVectorD *v;

  double Tbeam=0;
  double Tcosmics=0;
  double EOT=0;

  TH1D *hBeam;
  TH1D *hCosmics;

  TH1D *hBeamTmp;
  TH1D *hCosmicsTmp;
  
  for (int ii=0;ii<nFiles;ii++){
    f=new TFile(fnames[ii].c_str());
    v=(TVectorD*)f->Get("v");
    Tbeam+=(*v)[0];
    EOT+=(*v)[1];
    Tcosmics+=(*v)[2];
    
    if (ii==0){
      hBeam=(TH1D*)f->Get("hEneCrystalBeamTrg2");
      hCosmics=(TH1D*)f->Get("hEneCrystalCosmicsTrg2");
    }
    else{
      hBeamTmp=(TH1D*)f->Get("hEneCrystalBeamTrg2");
      hCosmicsTmp=(TH1D*)f->Get("hEneCrystalCosmicsTrg2");

      hBeam->Add(hBeamTmp);
      hCosmics->Add(hCosmicsTmp);
    }
  }

  hCosmics->Scale(Tbeam/Tcosmics);

  int bin1,bin2;
  bin1=hBeam->FindBin(600.);
  bin2=hBeam->FindBin(1990.);

  double integralCosmics,integralBeam;

  integralCosmics=hCosmics->Integral(bin1,bin2);
  integralBeam=hBeam->Integral(bin1,bin2);
  
  cout<<Tbeam<<" "<<EOT<<" "<<Tcosmics<<endl;

  cout<<integralBeam<<" "<<integralCosmics<<endl;

  hBeam->Draw();
  hCosmics->SetLineColor(2);
  hCosmics->Draw("HISTSAMES");
  
}
