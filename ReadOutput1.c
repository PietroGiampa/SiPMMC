//takes the output from RunMC.c and creates a graph of true PSD vs recorded PSD, detected photons vs energy, recorded PSD vs energy, and a histogram of residual

#include "MakeList.c"

void ReadOutput1(TString filename){

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
  SiPMmc->GetEntry(5); int tpb = constants.tpbOnOff;
  SiPMmc->GetEntry(6); TString evt_type = constants.recoil;

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

  //Graph1: true PSD vs recorded PSD
  TH2D *hPSD = new TH2D("hPSD","",100,-0.02,1.02,100,0,1);

  //Graph2: detected photons vs energy
  //not sure what the maximum photon number should be
  TH2D *hPhotons = new TH2D("hphotons", "", 100, energy_min, energy_max, 100, 0, 1300);

  //Graph3: recorded PSD vs energy
  TH2D *hPSDenergy = new TH2D("hPSDenergy", "", 100, energy_min, energy_max, 100, -0.02, 1.02);

  //Graph4: histogram of residual
  TH1D *hRes = new TH1D("hRes","",100, 0, 1);


  //loop through all of the events and add them to the graph
  for (int i=0; i<evt_max; i++){
     SiPMmc->GetEntry(i);
     hPSD->Fill(rec_psd,tru_psd);
     hPhotons->Fill(erecoil,n_coll_p);
     hPSDenergy->Fill(erecoil,rec_psd);
     hRes->Fill(residual);
  }


  //Graph1
  TCanvas *c1 = new TCanvas("c1","c1");
  hPSD->SetTitle("Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff+", Type: "+evt_type+"R");
  hPSD->GetXaxis()->SetTitle("Recorded PSD");
  hPSD->GetYaxis()->SetTitle("True PSD");
  hPSD->Draw();
  //add a y=x line
  TF1 *line1 = new TF1("line","x",-0.02,1.02);
  line1->Draw("same");

  //Graph2
  TCanvas *c2 = new TCanvas("c2","c2");
  hPhotons->SetTitle("Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff+", Type: "+evt_type+"R");
  hPhotons->GetXaxis()->SetTitle("Recoil energy (keV)");
  hPhotons->GetYaxis()->SetTitle("Number of collected photons");
  hPhotons->Draw();

  //Graph3
  TCanvas *c3 = new TCanvas("c3","c3");
  hPSDenergy->SetTitle("Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff+", Type: "+evt_type+"R");
  hPSDenergy->GetXaxis()->SetTitle("Recoil energy (keV)");
  hPSDenergy->GetYaxis()->SetTitle("Recorded PSD");
  hPSDenergy->Draw();

  //Graph4
  TCanvas *c4 = new TCanvas("c4", "c4");
  hRes->SetTitle("Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff+", Type: "+evt_type+"R");
  hRes->GetXaxis()->SetTitle("PSD Residual");
  hRes->GetYaxis()->SetTitle("number of events");
  hRes->Draw();


  //Step 4: save
  gSystem->Exec("mkdir Img/"+directory);
  c1->SaveAs("Img/"+directory+"/"+evt_type+"R__TruePSDvsRecPSD.png");
  c2->SaveAs("Img/"+directory+"/"+evt_type+"R__PhotonsVsEnergy.png");
  c3->SaveAs("Img/"+directory+"/"+evt_type+"R__RecPSDvsEnergy.png");
  c4->SaveAs("Img/"+directory+"/"+evt_type+"R__residual.png");
}

