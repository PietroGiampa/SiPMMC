//takes the output from RunMC.c and creates a graph of true PSD vs recorded PSD, detected photons vs energy, recorded PSD vs energy, a histogram of residual, and
//leakage vs energy

//If the data file comes from one simulation (there is one seed in the file name), don't use total_evts.
//If the data file is a combination of multiple simulations (there are multiple seeds in the file name), set total_evts to the sum of the events from all
//of the simulations (should be the first number in the file name).

#include "MakeList.c"

void ReadOutput1(TString filename, long total_evts=0){

  //Step 1: read in the data file
  TFile *fileIN = TFile::Open(filename);
  TTree *SiPMmc = (TTree*)fileIN->Get("SiPMmc");

  //fetch the necessary stuff from the data file
  Double_t tru_psd, rec_psd, residual, erecoil, leak_energy;
  Int_t n_coll_p;
  constant_list constants;
  SiPMmc->SetBranchAddress("tru_psd",&tru_psd);
  SiPMmc->SetBranchAddress("rec_psd",&rec_psd);
  SiPMmc->SetBranchAddress("residual",&residual);
  SiPMmc->SetBranchAddress("n_coll_p",&n_coll_p);
  SiPMmc->SetBranchAddress("erecoil",&erecoil);
  SiPMmc->SetBranchAddress("leak_energy",&leak_energy);
  SiPMmc->SetBranchAddress("constants", (Long64_t*)(&constants));

  //load the constants
  SiPMmc->GetEntry(0); long evt_max = constants.evts;
  if (total_evts==0) total_evts = evt_max;
  SiPMmc->GetEntry(1); double energy_min = constants.eMin;
  SiPMmc->GetEntry(2); double energy_max = constants.eMax;
  SiPMmc->GetEntry(3); double pde = constants.SiPM_pde;
  SiPMmc->GetEntry(4); double coll_eff = constants.light_cov;
  SiPMmc->GetEntry(5); double photo_yield = constants.yield;
  SiPMmc->GetEntry(6); int tpb = constants.tpbOnOff;
  SiPMmc->GetEntry(7); TString evt_type = constants.recoil;

  //Step 2: format
  //change constants into strings
  TString num = Form("%ld",total_evts);
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
  double max_ph = energy_max*photo_yield*coll_eff;
  TH2D *hPhotons = new TH2D("hphotons", "", 100, energy_min, energy_max, 100, 0, max_ph+50);

  //Graph3: recorded PSD vs energy
  TH2D *hPSDenergy = new TH2D("hPSDenergy", "", 100, energy_min, energy_max, 100, -0.02, 1.02);

  //Graph4: histogram of residual
  TH1D *hRes = new TH1D("hRes","",100, 0, 1);

  //Graph5: leakage vs energy
  TH1D *hLeak = new TH1D("hLeak","",100,energy_min,energy_max);
  TH1D *hEnergy = new TH1D("hEnergy","",100,energy_min,energy_max);


  //loop through all of the events and add them to the graph
  for (int i=0; i<total_evts; i++){
    SiPMmc->GetEntry(i);
    hPSD->Fill(rec_psd,tru_psd);
    hPhotons->Fill(erecoil,n_coll_p);
    hPSDenergy->Fill(erecoil,rec_psd);
    hRes->Fill(residual);
    hLeak->Fill(leak_energy);
    hEnergy->Fill(erecoil);
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

  //Graph5
  TH1D *hLeakEnergy = new TH1D(*hLeak);
  hLeakEnergy->Divide(hEnergy);
  TCanvas *c5 = new TCanvas("c5", "c5");
  hLeakEnergy->SetTitle("Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff+", Type: "+evt_type+"R");
  hLeakEnergy->GetXaxis()->SetTitle("Recoil energy (keV)");
  hLeakEnergy->GetYaxis()->SetTitle("Leakage");
  hLeakEnergy->Draw();

  //Step 4: save
  gSystem->Exec("mkdir Img/"+directory);
  c1->SaveAs("Img/"+directory+"/"+evt_type+"R__TruePSDvsRecPSD.png");
  c2->SaveAs("Img/"+directory+"/"+evt_type+"R__PhotonsVsEnergy.png");
  c3->SaveAs("Img/"+directory+"/"+evt_type+"R__RecPSDvsEnergy.png");
  c4->SaveAs("Img/"+directory+"/"+evt_type+"R__residual.png");
  if (evt_type=='E') c5->SaveAs("Img/"+directory+"/"+evt_type+"R__LeakageVsEnergy.png");
}

