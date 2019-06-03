//takes both ER and NR output from RunMC.c and creates a graph of true PSD vs recorded PSD, detected photons vs energy, and recorded PSD vs energy
//I'm assumeing that the ER and NR simulations have the same constants

void ReadOutput2(TString ER_filename, TString NR_filename){

  //Step 1: define the constants structure (first defined in RunMC.c)
  struct constant_list {
    long evts;
    double eMin;
    double eMax;
    double SiPM_pde;
    double light_cov;
    int tpbOnOff;
    char recoil;
  };


  //Step 2: read in the ER data file
  TFile *fileIN1 = TFile::Open(ER_filename);
  TTree *SiPMmc1 = (TTree*)fileIN1->Get("SiPMmc");
  //read in the NR data file
  TFile *fileIN2 = TFile::Open(NR_filename);
  TTree *SiPMmc2 = (TTree*)fileIN2->Get("SiPMmc");

  //fetch the necessary stuff from the ER data file
  Double_t epsd1, rpsd1, erecoil1;
  Int_t n_coll_p1;
  constant_list constants1;
  SiPMmc1->SetBranchAddress("epsd",&epsd1);
  SiPMmc1->SetBranchAddress("rpsd",&rpsd1);
  SiPMmc1->SetBranchAddress("n_coll_p",&n_coll_p1);
  SiPMmc1->SetBranchAddress("erecoil",&erecoil1);
  SiPMmc1->SetBranchAddress("constants", &constants1);
  //fetch the necessary stuff from the NR data file
  Double_t epsd2, rpsd2, erecoil2;
  Int_t n_coll_p2;
  SiPMmc2->SetBranchAddress("epsd",&epsd2);
  SiPMmc2->SetBranchAddress("rpsd",&rpsd2);
  SiPMmc2->SetBranchAddress("n_coll_p",&n_coll_p2);
  SiPMmc2->SetBranchAddress("erecoil",&erecoil2);

  //load the constants
  SiPMmc1->GetEntry(0); Long_t evt_max = constants1.evts;
  SiPMmc1->GetEntry(1); Double_t energy_min = constants1.eMin;
  SiPMmc1->GetEntry(2); Double_t energy_max = constants1.eMax;
  SiPMmc1->GetEntry(3); Double_t pde = constants1.SiPM_pde;
  SiPMmc1->GetEntry(4); Double_t coll_eff = constants1.light_cov;
  SiPMmc1->GetEntry(5); Int_t tpb = constants1.tpbOnOff;

  //Step 3: format
  //change constants into strings
  TString num = Form("%ld",evt_max);
  TString name_pde = Form("%fd",pde);
  TString name_coll_eff = Form("%fd",coll_eff);
  string OnOff;
  if (tpb==0){OnOff="off";}
  else if (tpb==1){OnOff="on";}

  //format file name to use later
  ER_filename.ReplaceAll("Data/","");
  ER_filename.ReplaceAll(".root","");

  //get rid of the stats box
  gStyle->SetOptStat(0);

  //Step 4: Graphing
  //Graph1: true PSD vs recorded PSD
  //making a graph of 100 bins from 0 to 1 on the x axis and 100 bins from 0 to 1 on the y axis
  TH2D *hPSD1 = new TH2D("hPSD1","",100,0,1,100,0,1);
  TH2D *hPSD2 = new TH2D("hPSD2","",100,0,1,100,0,1);

  //Graph2: detected photons vs energy
  //not sure what the maximum photon number should be
  TH2D *hPhotons1 = new TH2D("hphotons1", "", 100, energy_min, energy_max, 100, 0, 1300);
  TH2D *hPhotons2 = new TH2D("hphotons2", "", 100, energy_min, energy_max, 100, 0, 1300);

  //Graph3: recorded PSD vs energy
  TH2D *hPSDenergy1 = new TH2D("hPSDenergy1", "", 100, energy_min, energy_max, 100, 0, 1);
  TH2D *hPSDenergy2 = new TH2D("hPSDenergy2", "", 100, energy_min, energy_max, 100, 0, 1);


  //loop through all of the events and add them to the graphs
  for (int i=0; i<evt_max; i++){
    SiPMmc1->GetEntry(i);
    SiPMmc2->GetEntry(i);
    hPSD1->Fill(rpsd1,epsd1);
    hPSD2->Fill(rpsd2,epsd2);
    hPhotons1->Fill(erecoil1,n_coll_p1);
    hPhotons2->Fill(erecoil2,n_coll_p2);
    hPSDenergy1->Fill(erecoil1,rpsd1);
    hPSDenergy2->Fill(erecoil2,rpsd2);
  }

  //Graph1
  TCanvas *c1 = new TCanvas("c1","c1");
  hPSD1->SetTitle("Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff);
  hPSD1->GetXaxis()->SetTitle("Recorded PSD");
  hPSD1->GetYaxis()->SetTitle("True PSD");
  hPSD1->SetMarkerColor(kRed);
  hPSD1->SetFillColor(kRed);
  hPSD1->Draw();
  hPSD2->SetMarkerColor(kBlack);
  hPSD2->SetFillColor(kBlack);
  hPSD2->Draw("same");
  //add a y=x line
  TF1 *line1 = new TF1("line","x",0,1);
  line1->Draw("same");
  //add a legend
  auto leg1 = new TLegend(.13,.68,.3,.88);
  leg1->AddEntry(hPSD1, "ER", "F");
  leg1->AddEntry(hPSD2, "NR", "F");
  leg1->Draw("same");

  //Graph2
  TCanvas *c2 = new TCanvas("c2","c2");
  hPhotons1->SetTitle("Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff);
  hPhotons1->GetXaxis()->SetTitle("Recoil energy (keV)");
  hPhotons1->GetYaxis()->SetTitle("Number of collected photons");
  hPhotons1->SetMarkerColor(kRed);
  hPhotons1->Draw();
  hPhotons2->SetMarkerColor(kBlack);
  hPhotons2->Draw("same");
  leg1->Draw("same");

  //Graph3
  TCanvas *c3 = new TCanvas("c3","c3");
  hPSDenergy1->SetTitle("Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff);
  hPSDenergy1->GetXaxis()->SetTitle("Recoil energy (keV)");
  hPSDenergy1->GetYaxis()->SetTitle("Recorded PSD");
  hPSDenergy1->SetMarkerColor(kRed);
  hPSDenergy1->Draw();
  hPSDenergy2->SetMarkerColor(kBlack);
  hPSDenergy2->Draw("same");
  auto leg3 = new TLegend(.12,.28,.26,.11);
  leg3->AddEntry(hPSD1, "ER", "F");
  leg3->AddEntry(hPSD2, "NR", "F");
  leg3->Draw("same");


  //Save stuff
  c1->SaveAs("Img/"+ER_filename+"&NR__TruePSDvsRecPSD.png");
  c2->SaveAs("Img/"+ER_filename+"&NR__PhotonsVsEnergy.png");
  c3->SaveAs("Img/"+ER_filename+"&NR__RecPSDvsEnergy.png");

}
