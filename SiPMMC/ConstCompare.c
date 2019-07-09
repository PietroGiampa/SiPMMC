//TO RUN: define a TString array in the root terminal, then pass that into the function. (I don't know why it doesn't work otherwise)

//Plots multiple simulations, so you can compare the effects of different constants.
//Other than PDE and coll_eff, the files should have the same constants.
//most of this file is copied from ReadOutput.c
//currently have enough colours for 14 files

#include "MakeList.c"
//#include <math.h>
#include "nr50.c"

void ConstCompare(int len, TString files[len], long total_evts=0, TString var=""){

    //check that they've input total_evts correctly
    for (int i=0;i<len;i++){
	if (files[i].Contains("-") & (total_evts==0)){
	    cout << "If the data file is a combination of multiple simulations, you must set total_evts." << endl;
	    exit(EXIT_FAILURE);
	}
    }

    //check that they've input var correctly
    if ((var=="") & (len>1)){
	cout << "Are you varying PDE or LC?" << endl;
	exit(EXIT_FAILURE);
    }

    //Step 1: Plot the first file with title and axis labels
    //A: Read in the data file
    double leak_energy, tot_good_nrg, erecoil, rec_psd;
    int badPSD;
    constant_list constants;
    long evt_max;
    double energy_min, energy_max, photo_yield, pde, coll_eff, pdes[len], coll_effs[len];
    int tpb;
    TString evt_type, evt_types[len];
    TCanvas *c1, *c2, *c3;
    TH1D *hLeakEnergy;
    TLegend *leg1, *leg2;
    double fracs[len],fracErrs[len], zeros[len];
    double fracErr;
    TGraphErrors *ER = new TGraphErrors(); ER->SetLineColor(kRed); //stuff of the appropriate colour to add to the legend
    TGraphErrors *NR = new TGraphErrors(); NR->SetLineColor(kBlack);

    TFile *fileIN = TFile::Open(files[0]);
    TTree *SiPMmc = (TTree*)fileIN->Get("SiPMmc");
    SiPMmc->SetBranchAddress("rec_psd",&rec_psd);
    SiPMmc->SetBranchAddress("erecoil",&erecoil);
    SiPMmc->SetBranchAddress("tot_good_nrg",&tot_good_nrg);
    SiPMmc->SetBranchAddress("badPSD",&badPSD);
    SiPMmc->SetBranchAddress("constants", (Long64_t*)(&constants));

    SiPMmc->GetEntry(0); evt_max = constants.evts;
    if (total_evts==0) total_evts = evt_max;
    SiPMmc->GetEntry(1); energy_min = constants.eMin;
    SiPMmc->GetEntry(2); energy_max = constants.eMax;
    SiPMmc->GetEntry(3); pde = constants.SiPM_pde;
    SiPMmc->GetEntry(4); coll_eff = constants.light_cov;
    SiPMmc->GetEntry(5); photo_yield = constants.yield;
    SiPMmc->GetEntry(6); tpb = constants.tpbOnOff;
    SiPMmc->GetEntry(7); evt_type = constants.recoil;

    evt_types[0] = evt_type;
    pdes[0] = pde;
    coll_effs[0] = coll_eff;

    //B: format
    //change constants into strings
    TString num = Form("%ld",total_evts);
    TString name_pde = Form("%fd",pde);
    TString name_coll_eff = Form("%fd",coll_eff);
    string OnOff;
    if (tpb==0){OnOff="off";}
    else if (tpb==1){OnOff="on";}

    //get rid of the stats box
    gStyle->SetOptStat(0);
    Color_t colors[14] = {kRed,kYellow+1,kBlue,kGray+1,kMagenta,kBlack,kGreen,kOrange+7,kViolet-3,kOrange+4,kGreen+4,kCyan,kRed+4,kCyan+4};


    //C: graphing
    double numbins = (energy_max-energy_min)*2.0;
    TH1D *hLeak = new TH1D("hLeak","",numbins,energy_min,energy_max);
    TH1D *hEnergy = new TH1D("hEnergy","",numbins,energy_min,energy_max);

    double nr50;
    //loop through all of the events and add them to the graphs
    for (int i=0;i<total_evts;i++){
	SiPMmc->GetEntry(i);
	nr50 = exp_fit(&erecoil,pars);
	if (rec_psd>nr50 & rec_psd!=0 & rec_psd!=1) leak_energy = erecoil;
	else leak_energy = -1;
	hLeak->Fill(leak_energy);
	hEnergy->Fill(tot_good_nrg);
    }

    //Graph1: leakage vs energy
    c1 = new TCanvas("c1", "c1");
    c1->SetLogy();
    hLeak->Sumw2();
    hEnergy->Sumw2();
    hLeakEnergy = new TH1D(*hLeak);
    if (len>1){
      hLeakEnergy->SetMaximum(0.3);
      hLeakEnergy->SetMinimum(0.0000001);
    }
    hLeakEnergy->Divide(hEnergy);
//for (int i=1;i<numbins+1;i++) cout << hLeakEnergy->GetXaxis()->GetBinCenter(i) << ": " << hLeakEnergy->GetBinContent(i) << endl;
    hLeakEnergy->GetYaxis()->SetLimits(0,1);
    hLeakEnergy->SetLineColor(colors[0]);
    hLeakEnergy->GetXaxis()->SetTitle("Recoil energy (keV)");
    hLeakEnergy->GetYaxis()->SetTitle("Leakage");
    if (var=="PDE"){
	hLeakEnergy->SetTitle("Events: "+num+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff+", Type: "+evt_type+"R");
	leg1 = new TLegend(.65,.89-0.03*len,.89,.89);
	leg1->AddEntry(hLeakEnergy, "PDE: "+name_pde, "L");
    }
    else if (var=="LC"){
	hLeakEnergy->SetTitle("Events: "+num+", PDE: "+name_pde+", TPB: "+OnOff+", Type: "+evt_type+"R");
	leg1 = new TLegend(.12, .12, .35, .12+0.03*len);
	leg1->AddEntry(hLeakEnergy, " LC: "+name_coll_eff, "L");
    }
    else if (var==""){
	hLeakEnergy->SetTitle("Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff+", Type: "+evt_type+"R");
	leg1 = new TLegend(0.,0.,0.,0.);
    }
    if (evt_type=='E'){
	hLeakEnergy->Draw("E1");
	leg1->Draw("same");
    }

    //Graph2 & Graph3: fracBad vs pde/coll_eff
    double fracBad = double(badPSD)/double(total_evts);
    if (badPSD != 0) fracErr = fracBad*sqrt(1/badPSD + 1/total_evts);
    else fracErr = 0;
    fracs[0] = fracBad;
    fracErrs[0] = fracErr;
    zeros[0] = 0.0;
//I want the ER events to be red and the NR to be black. Maybe put them in separate vectors?

    //Step 2: read and plot the rest of the files
    for (int i=1;i<len;i++){
	//A: Read in the data file
	fileIN = TFile::Open(files[i]);
	SiPMmc = (TTree*)fileIN->Get("SiPMmc");
//	SiPMmc->SetBranchAddress("leak_energy",&leak_energy);
	SiPMmc->SetBranchAddress("erecoil",&erecoil);
	SiPMmc->SetBranchAddress("tot_good_nrg",&tot_good_nrg);
	SiPMmc->SetBranchAddress("badPSD",&badPSD);
	SiPMmc->SetBranchAddress("rec_psd",&rec_psd);
	SiPMmc->SetBranchAddress("constants", (Long64_t*)(&constants));

	SiPMmc->GetEntry(0); evt_max = constants.evts;
	if (total_evts==0) total_evts = evt_max;
	SiPMmc->GetEntry(1); energy_min = constants.eMin;
	SiPMmc->GetEntry(2); energy_max = constants.eMax;
	SiPMmc->GetEntry(3); pde = constants.SiPM_pde;
	SiPMmc->GetEntry(4); coll_eff = constants.light_cov;
	SiPMmc->GetEntry(5); photo_yield = constants.yield;
	SiPMmc->GetEntry(6); tpb = constants.tpbOnOff;
	SiPMmc->GetEntry(7); evt_type = constants.recoil;

	evt_types[i] = evt_type;
	pdes[i] = pde;
	coll_effs[i] = coll_eff;

	//check that the files have the same event type
	if (evt_types[i] != evt_types[i-1]){
		cout << "Files should have the same event type." << endl;
		exit(EXIT_FAILURE);
	}

	//B: format
	//change constants into strings
	num = Form("%ld",total_evts);
	name_pde = Form("%fd",pde);
	name_coll_eff = Form("%fd",coll_eff);
	if (tpb==0){OnOff="off";}
	else if (tpb==1){OnOff="on";}

	//C: graphing
	//Graph1: leakage vs energy
	hLeak = new TH1D("hLeak","",numbins,energy_min,energy_max); //get rid of previous graphs
	hEnergy = new TH1D("hEnergy","",numbins,energy_min,energy_max);
	for (int i=0;i<total_evts;i++){
	    SiPMmc->GetEntry(i);
            nr50 = exp_fit(&erecoil,pars);
            if (rec_psd>nr50 & rec_psd!=0 & rec_psd!=1) leak_energy = erecoil;
            else leak_energy = -1;
            hLeak->Fill(leak_energy);
            hEnergy->Fill(tot_good_nrg);
	}
	c1->cd();
	hLeak->Sumw2();
	hEnergy->Sumw2();
	hLeakEnergy = new TH1D(*hLeak);
	hLeakEnergy->Divide(hEnergy);
	hLeakEnergy->SetLineColor(colors[i]);
	if (evt_type=='E'){
	    hLeakEnergy->Draw("E1same");
	    if (var=="PDE") leg1->AddEntry(hLeakEnergy, "PDE: "+name_pde, "L");
	    else if (var=="LC") leg1->AddEntry(hLeakEnergy, " LC: "+name_coll_eff, "L");
	    leg1->Draw("same");
	}

	//Graph2 & Graph3: fracBad vs PDE/coll_eff
	fracBad = double(badPSD)/double(total_evts);
	if (badPSD != 0) fracErr = fracBad*sqrt(1/badPSD + 1/total_evts);
	else fracErr = 0;
 	fracs[i] = fracBad;
	fracErrs[i] = fracErr;
	zeros[i] = 0.0;

    }


    //Make graph2 and graph3
    //Graph2: fracBad vs pde
    c2 = new TCanvas("c2", "c2");
    for (int i=0;i<len;i++) if (fracs[i] != 0){
        c2->SetLogy();
        break;
    }
    TGraphErrors *fracPDE = new TGraphErrors(len, pdes, fracs, zeros, fracErrs);
    fracPDE->SetTitle("Events: "+num+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff+", Type: "+evt_type+"R");
    fracPDE->GetXaxis()->SetTitle("PDE");
    fracPDE->GetYaxis()->SetTitle("fraction of events with poorly reconstructed PSD");
    fracPDE->SetMarkerStyle(kFullCircle);
    fracPDE->Draw("APE");
//    leg2 = new TLegend(.70,.75,.87,.88);
//    leg2->AddEntry(ER, "ER", "L");
//    leg2->AddEntry(NR, "NR", "L");
//    leg2->Draw("same");

    //Graph3: fracBad vs coll_eff
    c3 = new TCanvas("c3", "c3");
    for (int i=0;i<len;i++) if (fracs[i] != 0){
	c3->SetLogy();
	break;
    }
    TGraphErrors *fracLC = new TGraphErrors(len, coll_effs, fracs, zeros, fracErrs);
    fracLC->SetTitle("Events: "+num+", PDE: "+name_pde+", TPB: "+OnOff+", Type: "+evt_type+"R");
    fracLC->GetXaxis()->SetLimits(0,1);
    fracLC->GetYaxis()->SetLimits(0,1);
    fracLC->GetXaxis()->SetTitle("Collection Efficiency");
    fracLC->GetYaxis()->SetTitle("fraction of events with poorly reconstructed PSD");
    fracLC->SetMarkerStyle(kFullCircle);
    fracLC->Draw("APE");
//    leg2->Draw("same");


    //Step 3: save stuff

    //figure out if pde or coll_eff was varied
//    int var_pde = 0, var_coll_eff = 0;
//    if (len>1){
//	for (int i=1;i<len;i++) if (pdes[i] != pdes[i-1]){
//	    var_pde = 1;
//	    break;
//	}
//	for (int i=1;i<len;i++) if (coll_effs[i] != coll_effs[i-1]){
//	    var_coll_eff = 1;
//	    break;
//	}
//    }
    TString var_string = "";
//    if (var_pde==1 & var_coll_eff==0) var = "_varPDE";
//    else if (var_coll_eff==1 & var_pde==0) var = "_varLC";
    if (var=="PDE") var_string = "_varPDE";
    if (var=="LC") var_string = "_varLC";

    //check how many ER files there were
    int ERs = 0;
    for (int i=0;i<len;i++) if (evt_types[i]=='E') ERs +=1;

    gSystem->Exec("mkdir Img/ConstCompare/"+num);
    if (ERs > 0) c1->SaveAs("Img/ConstCompare/"+num+"/TPB"+OnOff+"_files"+ERs+var_string+"__LeakageVsEnergy.png");
    c2->SaveAs("Img/ConstCompare/"+num+"/TPB"+OnOff+"_files"+len+var_string+"__badPSDvsPDE.png");
    c3->SaveAs("Img/ConstCompare/"+num+"/TPB"+OnOff+"_files"+len+var_string+"__badPSDvsLC.png");
}
