//Takes one or two data files from RunMC.c.
//With two files, assumes that both have the same constants.
//If each data file comes from one simulation (there is one seed in the file name), don't use total_evts.
//If each data file is a combination of multiple simulations (there are multiple seeds in the file name), set total_evts to the sum of the events from all
//	of the simulations (should be the first number in the file name).
//Default graph type is scatter plot. Set heat_map==1 to make a heat map.
//If inputting only NR, you must pass "" to ER.

//Creates a graph of true PSD vs recorded PSD, detected photons vs energy, recorded PSD vs energy, a histogram of residual, and leakage vs energy. Calculates the
//	fraction of events that have a PSD of zero or 1.

#include "MakeList.c"
#include "SingletToTripletYALE.c"
#include "GetFloatAsString.c"

void ReadOutput(TString ER="", TString NR="", long total_evts=0, int heat_map=0){

  int numFiles;
  if (ER == "" & NR == ""){
    cout << "Please input at least one file." << endl;
    exit(EXIT_FAILURE);}
  else if (ER == "" | NR == "") numFiles = 1;
  else numFiles = 2;

  //check that they've input total_evts correctly
  if ((ER.Contains("-") | NR.Contains("-")) & (total_evts==0)){
    cout << "If the data file is a combination of multiple simulations, you must set total_evts." << endl;
    exit(EXIT_FAILURE);}

  //Step 1: Read in the data files
  //Define the data variables
  Double_t tru_psd1, rec_psd1, residual1, erecoil1, leak_energy1, tot_good_nrg1;
  Int_t n_coll_p1, badPSD1;
  constant_list constants1;
  Double_t tru_psd2, rec_psd2, residual2, erecoil2;
  Int_t n_coll_p2, badPSD2;
  constant_list constants2;
  Long_t evt_max;
  Double_t energy_min;
  Double_t energy_max;
  Double_t pde;
  Double_t coll_eff;
  Double_t photo_yield;
  Int_t tpb;
  TString evt_type;

  //read in the ER data file
  TTree *SiPMmc1;
  TTree *SiPMmc2;
  if (ER != ""){
    TFile *fileIN1 = TFile::Open(ER);
    SiPMmc1 = (TTree*)fileIN1->Get("SiPMmc");
    //fetch the necessary stuff from the ER data file
    SiPMmc1->SetBranchAddress("tru_psd",&tru_psd1);
    SiPMmc1->SetBranchAddress("rec_psd",&rec_psd1);
    SiPMmc1->SetBranchAddress("residual",&residual1);
    SiPMmc1->SetBranchAddress("n_coll_p",&n_coll_p1);
    SiPMmc1->SetBranchAddress("erecoil",&erecoil1);
    SiPMmc1->SetBranchAddress("leak_energy",&leak_energy1);
    SiPMmc1->SetBranchAddress("tot_good_nrg",&tot_good_nrg1);
    SiPMmc1->SetBranchAddress("badPSD",&badPSD1);
    SiPMmc1->SetBranchAddress("constants", (Long64_t*)(&constants1));
    //load the constants
    //I'm not sure if the SiPMmc1->GetEntry(n) is necessary, but as long as it works I won't change it.
    //if using two files or only using ER, use the constants from ER
    SiPMmc1->GetEntry(0); evt_max = constants1.evts;
    if (total_evts==0) total_evts = evt_max;
    SiPMmc1->GetEntry(1); energy_min = constants1.eMin;
    SiPMmc1->GetEntry(2); energy_max = constants1.eMax;
    SiPMmc1->GetEntry(3); pde = constants1.SiPM_pde;
    SiPMmc1->GetEntry(4); coll_eff = constants1.light_cov;
    SiPMmc1->GetEntry(5); photo_yield = constants1.yield;
    SiPMmc1->GetEntry(6); tpb = constants1.tpbOnOff;
    SiPMmc1->GetEntry(7); evt_type = constants1.recoil;
  }

  //read in the NR data file
  if (NR != ""){
    TFile *fileIN2 = TFile::Open(NR);
    SiPMmc2 = (TTree*)fileIN2->Get("SiPMmc");
    //fetch the necessary stuff from the NR data file
    SiPMmc2->SetBranchAddress("tru_psd",&tru_psd2);
    SiPMmc2->SetBranchAddress("rec_psd",&rec_psd2);
    SiPMmc2->SetBranchAddress("residual",&residual2);
    SiPMmc2->SetBranchAddress("n_coll_p",&n_coll_p2);
    SiPMmc2->SetBranchAddress("erecoil",&erecoil2);
    SiPMmc2->SetBranchAddress("badPSD",&badPSD2);
    SiPMmc2->SetBranchAddress("constants", (Long64_t*)(&constants2));
    //load the constants
    //if you're only using NR, use these constants
    if (ER == ""){
      SiPMmc2->GetEntry(0); Long_t evt_max = constants2.evts;
      if (total_evts==0) total_evts = evt_max;
      SiPMmc2->GetEntry(1); energy_min = constants2.eMin;
      SiPMmc2->GetEntry(2); energy_max = constants2.eMax;
      SiPMmc2->GetEntry(3); pde = constants2.SiPM_pde;
      SiPMmc2->GetEntry(4); coll_eff = constants2.light_cov;
      SiPMmc2->GetEntry(5); photo_yield = constants2.yield;
      SiPMmc2->GetEntry(6); tpb = constants2.tpbOnOff;
      SiPMmc2->GetEntry(7); evt_type = constants2.recoil;
    }
  }

  //Check that they've passed NR correctly
  if ((NR=="") & (evt_type=='N')){
    cout << "If inputting only NR, you must pass \"\" to ER." << endl;
    exit(EXIT_FAILURE);}


  //Step 2: format
  //change constants into strings
  TString num = Form("%ld",total_evts);
  TString name_pde = Form("%fd",pde);
  TString name_coll_eff = Form("%fd",coll_eff);
  string OnOff;
  if (tpb==0){OnOff="off";}
  else if (tpb==1){OnOff="on";}

  //format file name to use later
  TString directory;
  if (ER != ""){
    directory = ER;
    directory.ReplaceAll("_ER","");
  }
  else{
    directory = NR;
    directory.ReplaceAll("_NR","");
  }

  directory.ReplaceAll("Data/","");
  directory.ReplaceAll(".root","");

  //get rid of the stats box
  gStyle->SetOptStat(0);


  //Step 3: Graphing
  //Graph1: true PSD vs recorded PSD
  TH2D *hPSD1 = new TH2D("hPSD1","",100,-0.02,1.02,100,0,1);
  //Graph2: detected photons vs energy
  double max_ph = energy_max*photo_yield*coll_eff;
  TH2D *hPhotons1 = new TH2D("hphotons1", "", 100, energy_min, energy_max, 100, 0, max_ph+50);
  //Graph3: recorded PSD vs energy
  TH2D *hPSDenergy1 = new TH2D("hPSDenergy1", "", 100, energy_min, energy_max, 100, -0.02, 1.02);
  //Graph4: histogram of residual
  TH1D *hRes1 = new TH1D("hRes1","",100, 0, 1);
  //Graph5: leakage vs energy
  double numbins = (energy_max-energy_min)*2.0;
  TH1D *hLeak = new TH1D("hLeak","",numbins,energy_min,energy_max);
  TH1D *hEnergy = new TH1D("hEnergy","",numbins,energy_min,energy_max);

  TH2D *hPSD2;
  TH2D *hPhotons2;
  TH2D *hPSDenergy2;
  TH1D *hRes2;
  if (numFiles==2){
    hPSD2 = new TH2D("hPSD2","",100,-0.02,1.02,100,0,1);
    hPhotons2 = new TH2D("hphotons2", "", 100, energy_min, energy_max, 100, 0, max_ph+50);
    hPSDenergy2 = new TH2D("hPSDenergy2", "", 100, energy_min, energy_max, 100, -0.02, 1.02);
    hRes2 = new TH1D("hRes2","",100, 0, 1);
  }


  //loop through all of the events and add them to the graphs
  for (int i=0; i<total_evts; i++){
    if (ER != ""){
      SiPMmc1->GetEntry(i);
      hPSD1->Fill(rec_psd1,tru_psd1);
      hPhotons1->Fill(erecoil1,n_coll_p1);
      hPSDenergy1->Fill(erecoil1,rec_psd1);
      hRes1->Fill(residual1);
      hLeak->Fill(leak_energy1);
      hEnergy->Fill(tot_good_nrg1);
    }
    if (numFiles==2){
      SiPMmc2->GetEntry(i);
      hPSD2->Fill(rec_psd2,tru_psd2);
      hPhotons2->Fill(erecoil2,n_coll_p2);
      hPSDenergy2->Fill(erecoil2,rec_psd2);
      hRes2->Fill(residual2);
    }
    else if (ER == ""){ //NR != ""
      SiPMmc2->GetEntry(i);
      hPSD1->Fill(rec_psd2,tru_psd2);
      hPhotons1->Fill(erecoil2,n_coll_p2);
      hPSDenergy1->Fill(erecoil2,rec_psd2);
      hRes1->Fill(residual2);
    }
  }


  //Graph1
  TCanvas *c1 = new TCanvas("c1","c1");
  if (ER != ""){
    hPSD1->SetMarkerColor(kRed);
    hPSD1->SetFillColor(kRed);}
  hPSD1->GetXaxis()->SetTitle("Recorded PSD");
  hPSD1->GetYaxis()->SetTitle("True PSD");
  if (numFiles==1){
    hPSD1->SetTitle("Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff+", Type: "+evt_type+"R");
    if (heat_map==1){
      if (ER != "") gStyle->SetPalette(kSolar);
      else if (NR != "") gStyle -> SetPalette(kBird);
      hPSD1->Draw("COLZ");}
    else hPSD1->Draw();
  }
  TLegend *leg1;
  if (numFiles==2){
    hPSD1->SetTitle("Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff);
    if (heat_map==1){
      gStyle->SetPalette(kSolar);
      hPSD1->Draw("COLZ");}
    else hPSD1->Draw();
    hPSD2->SetMarkerColor(kBlack);
    hPSD2->SetFillColor(kBlack);
    hPSD2->Draw("same");
    //add a legend
    leg1 = new TLegend(.13,.68,.3,.88);
    leg1->AddEntry(hPSD1, "ER", "F");
    leg1->AddEntry(hPSD2, "NR", "F");
    leg1->Draw("same");
  }
  //add a y=x line
  TF1 *line1 = new TF1("line","x",-0.02,1.02);
  line1->SetLineColor(kBlue);
  line1->Draw("same");

  //Graph2
  TCanvas *c2 = new TCanvas("c2","c2");
  hPhotons1->GetXaxis()->SetTitle("Recoil energy (keV)");
  hPhotons1->GetYaxis()->SetTitle("Number of collected photons");
  if (ER != "") hPhotons1->SetMarkerColor(kRed);
  if (numFiles==1){
    hPhotons1->SetTitle("Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff+", Type: "+evt_type+"R");
    hPhotons1->Draw();}
  if (numFiles==2){
    hPhotons1->SetTitle("Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff);
    hPhotons1->Draw();
    hPhotons2->SetMarkerColor(kBlack);
    hPhotons2->Draw("same");
    leg1->Draw("same");
  }

  //Graph3
  TCanvas *c3 = new TCanvas("c3","c3");
  hPSDenergy1->GetXaxis()->SetTitle("Recoil energy (keV)");
  hPSDenergy1->GetYaxis()->SetTitle("Recorded PSD");
  if (ER != "") hPSDenergy1->SetMarkerColor(kRed);
  //if (numFiles==1)
  if (numFiles==1){
    hPSDenergy1->SetTitle("Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff+", Type: "+evt_type+"R");
    if (heat_map==1){
      hPSDenergy1->Draw("COLZ");
    }
    else hPSDenergy1->Draw();
  }
  //add a SingletToTripletYale line; it's stepped because that's how I implemented the leakage cut.
  double energyLine[2*size];
  double nuclearLine[2*size];
  for (int i=0;i<size;i++){
    energyLine[2*i] = energy[i]-0.5;
    energyLine[2*i + 1] = energy[i]+0.5;
    nuclearLine[2*i] = nuclear[i];
    nuclearLine[2*i + 1] = nuclear[i];
  }
  TGraph *line3 = new TGraph(2*size, energyLine, nuclearLine);
  line3->SetLineColor(kBlue);
  line3->SetLineWidth(3);
  line3->Draw("same");
  //if (numFiles==2)
  TLegend *leg3;
  if (numFiles==2){
    hPSDenergy1->SetTitle("Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff);
    if (heat_map==1){
      gStyle->SetPalette(kSolar);
      hPSDenergy1->Draw("COLZ");
    }
    else hPSDenergy1->Draw();
//why does it not look transparent??
    hPSDenergy2->SetMarkerColorAlpha(kBlack, 0.5);
    hPSDenergy2->Draw("same");
    leg3 = new TLegend(.88,.35,.70,.11);
    leg3->AddEntry(hPSD1, "ER", "F");
    leg3->AddEntry(hPSD2, "NR", "F");
    leg3->AddEntry(line3, "50% NR", "L");
    leg3->Draw("same");
  }

  //Graph4
  TCanvas *c4 = new TCanvas("c4", "c4");
  hRes1->GetXaxis()->SetTitle("PSD Residual");
  hRes1->GetYaxis()->SetTitle("number of events");
  if (ER != "") hRes1->SetLineColor(kRed);
  else hRes1->SetLineColor(kBlack);
  if (numFiles==1){
    hRes1->SetTitle("Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff+", Type: "+evt_type+"R");
    hRes1->Draw();}
  //for two files, NR has to be drawn first to set the y-axis size, because its peak is taller.
  TLegend *leg4;
  if (numFiles==2){
    hRes1->SetTitle("Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff);
    hRes2->SetLineColor(kBlack);
    hRes2->Draw();
    leg4 = new TLegend(.88,.72,.74,.89);
    leg4->AddEntry(hPSD1, "ER", "F");
    leg4->AddEntry(hPSD2, "NR", "F");
    leg4->Draw("same");
    hRes1->Draw("same");
  }

  //Graph5
  TCanvas *c5 = new TCanvas("c5", "c5");
  if (ER != ""){
    TH1D *hLeakEnergy = new TH1D(*hLeak);
    hLeakEnergy->Divide(hEnergy);
    hLeakEnergy->SetTitle("Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff+", Type: "+evt_type+"R");
    hLeakEnergy->GetXaxis()->SetTitle("Recoil energy (keV)");
    hLeakEnergy->GetYaxis()->SetTitle("Leakage");
    hLeakEnergy->Draw();
  }

  //Fraction of events with PSD zero or 1
  TCanvas *c6 = new TCanvas("c6", "c6");
  double fracBadER = double(badPSD1)/double(total_evts);
  TString name_fracBadER;
  double fracBadNR = double(badPSD2)/double(total_evts);
  TString name_fracBadNR;
  if (ER != ""){
    name_fracBadER = GetFloatAsString(fracBadER);
    cout << "The fraction of ER events with PSD zero or one is " << fracBadER << endl;
//why isn't this showing up?
    TLatex text1(0.25, 0.25, "The fraction of ER events with PSD zero or one is "+name_fracBadER);
    text1.Draw();
  }
  if (NR != ""){
    name_fracBadNR = GetFloatAsString(fracBadNR);
    cout << "The fraction of NR events with PSD zero or one is " << fracBadNR << endl;
    TLatex text2(0.25, 0.25, "The fraction of NR events with PSD zero or one is "+name_fracBadNR);
    text2.Draw();
  }

  //Step 4: Save stuff
  gSystem->Exec("mkdir Img/"+directory);
  if (numFiles==1){
    c2->SaveAs("Img/"+directory+"/"+evt_type+"R__PhotonsVsEnergy.png");
    c4->SaveAs("Img/"+directory+"/"+evt_type+"R__residual.png");
    if (evt_type=='E') c5->SaveAs("Img/"+directory+"/"+evt_type+"R__LeakageVsEnergy.png");
    if (evt_type=='E') c6->SaveAs("Img/"+directory+"/"+evt_type+"R__fracBad"+name_fracBadER+".png");
    else if (evt_type=='N') c6->SaveAs("Img/"+directory+"/"+evt_type+"R__fracBad"+name_fracBadNR+".png");
    //add 'heatMap' to the file name if the graph is a heat map
    if (heat_map==0){
      c1->SaveAs("Img/"+directory+"/"+evt_type+"R__TruePSDvsRecPSD.png");
      c3->SaveAs("Img/"+directory+"/"+evt_type+"R__RecPSDvsEnergy.png");
    }
    else if (heat_map==1){
      c1->SaveAs("Img/"+directory+"/"+evt_type+"R__TruePSDvsRecPSD_heatMap.png");
      c3->SaveAs("Img/"+directory+"/"+evt_type+"R__RecPSDvsEnergy_heatMap.png");
    }
  }
  else { //numfiles==2
    c1->SaveAs("Img/"+directory+"/ER&NR__TruePSDvsRecPSD.png");
    c2->SaveAs("Img/"+directory+"/ER&NR__PhotonsVsEnergy.png");
    c3->SaveAs("Img/"+directory+"/ER&NR__RecPSDvsEnergy.png");
    c4->SaveAs("Img/"+directory+"/ER&NR__residual.png");
    c5->SaveAs("Img/"+directory+"/"+evt_type+"R__LeakageVsEnergy.png");
    c6->SaveAs("Img/"+directory+"/frazBadER"+name_fracBadER+"NR"+name_fracBadNR+".png");
    //add 'heatMap' to the file name if the graph is a heat map
    if (heat_map==0){
      c1->SaveAs("Img/"+directory+"/ER&NR__TruePSDvsRecPSD.png");
      c3->SaveAs("Img/"+directory+"/ER&NR__RecPSDvsEnergy.png");
    }
    else if (heat_map==1){
      c1->SaveAs("Img/"+directory+"/ER&NR__TruePSDvsRecPSD_heatMap.png");
      c3->SaveAs("Img/"+directory+"/ER&NR__RecPSDvsEnergy_heatMap.png");
    }
  }
}
