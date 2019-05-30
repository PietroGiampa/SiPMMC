//takes the output from RunMC.c and creates a graph of true PSD vs recorded PSD, detected photons vs energy, and recorded PSD vs energy

#include "constants.c"

void ReadOutput(TString filename){

  TFile *fileIN = TFile::Open(filename);
  TTree *SiPMmc = (TTree*)fileIN->Get("SiPMmc");

  Double_t epsd, rpsd, erecoil;
  Int_t n_coll_p;
  SiPMmc->SetBranchAddress("epsd",&epsd);
  SiPMmc->SetBranchAddress("rpsd",&rpsd);
  SiPMmc->SetBranchAddress("n_coll_p",&n_coll_p);
  SiPMmc->SetBranchAddress("erecoil",&erecoil);

  //format file name to use later
  //remove the directory name
  filename.ReplaceAll("Data/","");
  //remove the .root
  filename.ReplaceAll(".root","");

  // Graph1: true PSD vs recorded PSD
  //making a graph of 100 bins from 0 to 1 on the x axis and 100 bins from 0 to 1 on the y axis
  TH2D *hPSD = new TH2D("hPSD","",100,0,1,100,0,1);

  //loop through all of the events and add them to the graph
  for (int i=0; i<evt_max; i++){
     SiPMmc->GetEntry(i);
     hPSD->Fill(rpsd,epsd);
  }

  //graph the data
  TCanvas *c1 = new TCanvas("c1","c1");
  hPSD->SetTitle(filename);
  hPSD->GetXaxis()->SetTitle("Recorded PSD");
  hPSD->GetYaxis()->SetTitle("True PSD");
  hPSD->Draw();
  //add a y=x line
  TF1 *line1 = new TF1("line","x",0,1);
  line1->Draw("same");

  //save the image
  c1->SaveAs("Img/"+filename+"__TruePSDvsRecPSD.png");


  //Graph2: detected photons vs energy
  //not sure what the maximum photon number should be
  TH2D *hPhotons = new TH2D("hphotons", "", 100, energy_min, energy_max, 100, 0, 1300);

  for (int i=0;i<evt_max;i++){
    SiPMmc->GetEntry(i);
    hPhotons->Fill(erecoil,n_coll_p);
  }

  TCanvas *c2 = new TCanvas("c2","c2");
  hPhotons->SetTitle(filename);
  hPhotons->GetXaxis()->SetTitle("Recoil energy");
  hPhotons->GetYaxis()->SetTitle("Number of collected photons");
  hPhotons->Draw();

  c2->SaveAs("Img/"+filename+"__PhotonsVsEnergy.png");

  //Graph3: recorded PSD vs energy
  TH2D *hPSDenergy = new TH2D("hPSDenergy", "", 100, energy_min, energy_max, 100, 0, 1);

  for (int i=0;i<evt_max;i++){
    SiPMmc->GetEntry(i);
    hPSDenergy->Fill(erecoil,rpsd);
  }

  TCanvas *c3 = new TCanvas("c3","c3");
  hPSDenergy->SetTitle(filename);
  hPSDenergy->GetXaxis()->SetTitle("Recoil energy");
  hPSDenergy->GetYaxis()->SetTitle("Recorded PSD");
  hPSDenergy->Draw();

  c3->SaveAs("Img/"+filename+"__RecPSDvsEnergy.png");

}

