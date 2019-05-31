//takes the output from RunMC.c and creates a graph of true PSD vs recorded PSD, detected photons vs energy, and recorded PSD vs energy

void ReadOutput(TString filename){

  //define the constants structure
  struct constant_list {
    long evts;
    double eMin;
    double eMax;
  };

  //read in the data file
  TFile *fileIN = TFile::Open(filename);
  TTree *SiPMmc = (TTree*)fileIN->Get("SiPMmc");

  //fetch the necessary stuff from the data file
  Double_t epsd, rpsd, erecoil;
  Int_t n_coll_p;
  constant_list constants;
  SiPMmc->SetBranchAddress("epsd",&epsd);
  SiPMmc->SetBranchAddress("rpsd",&rpsd);
  SiPMmc->SetBranchAddress("n_coll_p",&n_coll_p);
  SiPMmc->SetBranchAddress("erecoil",&erecoil);
  SiPMmc->SetBranchAddress("constants", &constants);

  SiPMmc->GetEntry(0);
  Long_t evt_max = constants.evts;
  SiPMmc->GetEntry(1);
  Double_t energy_min = constants.eMin;
  SiPMmc->GetEntry(2);
  Double_t energy_max = constants.eMax;

  //format file name to use later
  filename.ReplaceAll("Data/","");
  filename.ReplaceAll(".root","");

  //get rid of the stats box
  gStyle->SetOptStat(0);


  // Graph1: true PSD vs recorded PSD
  //making a graph of 100 bins from 0 to 1 on the x axis and 100 bins from 0 to 1 on the y axis
  TH2D *hPSD = new TH2D("hPSD","",100,0,1,100,0,1);

  //Graph2: detected photons vs energy
  //not sure what the maximum photon number should be
  TH2D *hPhotons = new TH2D("hphotons", "", 100, energy_min, energy_max, 100, 0, 1300);

  //Graph3: recorded PSD vs energy
  TH2D *hPSDenergy = new TH2D("hPSDenergy", "", 100, energy_min, energy_max, 100, 0, 1);


  //loop through all of the events and add them to the graph
  for (int i=0; i<evt_max; i++){
     SiPMmc->GetEntry(i);
     hPSD->Fill(rpsd,epsd);
     hPhotons->Fill(erecoil,n_coll_p);
     hPSDenergy->Fill(erecoil,rpsd);
  }


  //graph the data
  //Graph1
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

  //Graph2
  TCanvas *c2 = new TCanvas("c2","c2");
  hPhotons->SetTitle(filename);
  hPhotons->GetXaxis()->SetTitle("Recoil energy (keV)");
  hPhotons->GetYaxis()->SetTitle("Number of collected photons");
  hPhotons->Draw();

  c2->SaveAs("Img/"+filename+"__PhotonsVsEnergy.png");

  //Graph3
  TCanvas *c3 = new TCanvas("c3","c3");
  hPSDenergy->SetTitle(filename);
  hPSDenergy->GetXaxis()->SetTitle("Recoil energy (keV)");
  hPSDenergy->GetYaxis()->SetTitle("Recorded PSD");
  hPSDenergy->Draw();

  c3->SaveAs("Img/"+filename+"__RecPSDvsEnergy.png");

}

