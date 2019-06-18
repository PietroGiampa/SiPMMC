//takes the output from RunMC.c and creates a graph of true psd vs number of scintillation photons and recorded psd vs numbero of scintillation photons
//this file was just created for debugging, so it's not kept up to date.

#include "MakeList.c"

void ReadOutput3(TString filename){

  //Step 1: read in the data file
  TFile *fileIN = TFile::Open(filename);
  TTree *SiPMmc = (TTree*)fileIN->Get("SiPMmc");

  //fetch the necessary stuff from the data file
  Double_t tru_psd, rec_psd, residual, erecoil;
  Int_t n_coll_p;
  constant_list constants;
  SiPMmc->SetBranchAddress("tru_psd",&tru_psd);
  SiPMmc->SetBranchAddress("rec_psd",&rec_psd);
  SiPMmc->SetBranchAddress("residual",&residual);
  SiPMmc->SetBranchAddress("n_coll_p",&n_coll_p);
  SiPMmc->SetBranchAddress("erecoil",&erecoil);
  SiPMmc->SetBranchAddress("constants", (Long64_t*)(&constants));

  //load the constants
  SiPMmc->GetEntry(0); long evt_max = constants.evts;
  SiPMmc->GetEntry(1); double energy_min = constants.eMin;
  SiPMmc->GetEntry(2); double energy_max = constants.eMax;
  SiPMmc->GetEntry(3); double pde = constants.SiPM_pde;
  SiPMmc->GetEntry(4); double coll_eff = constants.light_cov;
  SiPMmc->GetEntry(5); double photo_yield = constants.yield;
  SiPMmc->GetEntry(6); int tpb = constants.tpbOnOff;
  SiPMmc->GetEntry(7); TString evt_type = constants.recoil;

  //Step 2: format
  //change constants into strings
  TString num = Form("%ld",evt_max);
  TString name_pde = Form("%fd",pde);
  TString name_coll_eff = Form("%fd",coll_eff);
  string OnOff;
  if (tpb==0){OnOff="off";}
  else if (tpb==1){OnOff="on";}

  //format file name to use later
  TString directory = filename;
  directory.ReplaceAll("Data/","");
  directory.ReplaceAll(".root","");
  directory.ReplaceAll("_NR","");
  directory.ReplaceAll("_ER","");

  //get rid of the stats box
  gStyle->SetOptStat(0);


  //Step 3: Graphing

  if (evt_type=='E'){gStyle->SetMarkerColor(kRed);}

  //Graph1: true PSD vs number of photons
  double max_ph = energy_max*photo_yield*coll_eff;
  TH2D *hTru = new TH2D("hTru","",100,0,max_ph+50,100,-0.02,1.02);

  //Graph2: rec_psd vs number of photons
  TH2D *hRec = new TH2D("hRec","",100,0,max_ph+50,100,-0.02,1.02);


  //loop through all of the events and add them to the graph
  for (int i=0; i<evt_max; i++){
     SiPMmc->GetEntry(i);
     hTru->Fill(n_coll_p,tru_psd);
     hRec->Fill(n_coll_p,rec_psd);
  }


  //Graph1
  TCanvas *c1 = new TCanvas("c1","c1");
  hTru->SetTitle("Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff+", Type: "+evt_type+"R");
  hTru->GetXaxis()->SetTitle("Number of collected photons");
  hTru->GetYaxis()->SetTitle("True PSD");
  hTru->Draw();

  //Graph2
  TCanvas *c2 = new TCanvas("c2","c2");
  hRec->SetTitle("Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff+", Type: "+evt_type+"R");
  hRec->GetXaxis()->SetTitle("Number of collected photons");
  hRec->GetYaxis()->SetTitle("Recorded PSD");
  hRec->Draw();

  //Step 4: save
  gSystem->Exec("mkdir Img/"+directory);
  c1->SaveAs("Img/"+directory+"/"+evt_type+"R__TruePSDvsPhotons.png");
  c2->SaveAs("Img/"+directory+"/"+evt_type+"R__RecPSDvsPhotons.png");

}

