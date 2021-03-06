//---------------------------------------//
// Pietro Giampa, TRIUMF, 2018           //
// Edited by Frankie Polak, TRIUMF, 2019 //
//---------------------------------------//
//---------------------------------------//
// How to run the MC:                    //
//   1) long evt_max, number of          //
//      simulated events                 //
//   2) int IsTPBon, set to 1 if there   //
//      is TPB, 0 if there is not        //
//   3) int seed: pick a number between  //
//      −32,767 and 32,767               //
//   4) TString evt_type: NR or ER       //
//   5) edit other simulation parameters //
//      in constants.c                   //
//   6) run ReadOutput.c on the data     //
//      files to graph results           //
//                                       //
// root -l 'RunMC(<parameters>)'         //
//---------------------------------------//

//Include Function
#include "constants.c"
#include "DecayFunction.c"
#include "LArEmissionSpectrum.c"
#include "MakeList.c"
#include "SiPMcdpPDF.c"
#include "SiPMcdpHist.c"
#include "GetFloatAsString.c"
#include "SiPMTimeResolution.c"
#include "SingletToTripletFunction.c"
#include "SingletToTripletYALE.c"
#include "SingletToTripletSCENE.c"

void RunMC(long evt_max, int IsTPBon, int seed, TString evt_type)
{

  //--------------------------------------------------------------------//
  // Stage 1, Set Up                                                    //
  //--------------------------------------------------------------------//
  //Get the LAr emission Spectrum
  //Where are these parameters from?
  TF1 *LArEmission = new TF1("LArEmission",LArEmissionSpectrum,100,300,12);
  LArEmission->SetParameter(1,128);
  LArEmission->SetParameter(2,5);
  LArEmission->SetParameter(0,0.999);
  LArEmission->SetParameter(4,158);
  LArEmission->SetParameter(5,4);
  LArEmission->SetParameter(3,0.0002);
  LArEmission->SetParameter(7,195);
  LArEmission->SetParameter(8,30);
  LArEmission->SetParameter(6,0.0002);
  LArEmission->SetParameter(10,272);
  LArEmission->SetParameter(11,20);
  LArEmission->SetParameter(9,0.0002);

  //Set UP the true PSD response function for NR and ER
  TF1 *NRfunc = new TF1("NRfunc",SingletToTripletFunction,0.0,100.0,3);
  NRfunc->SetParameter(1,100);
  NRfunc->SetParameter(2,0.7);
  NRfunc->SetParameter(0,10.0);
  TF1 *ERfunc =new TF1("ERfunc",SingletToTripletFunction,0.0,100.0,3);
  ERfunc->SetParameter(1,100);
  ERfunc->SetParameter(2,0.3);
  ERfunc->SetParameter(0,10.0);

  //Set up the two scintillation time PDFs
  TF1 *fastPDF = new TF1("fastPDF",DecayFunction,0,400,2);
  fastPDF->SetParameter(0,1.0);
  fastPDF->SetParameter(1,LAr_fast_t);
  TF1 *slowPDF = new TF1("slowPDF",DecayFunction,0,time_window,2);
  slowPDF->SetParameter(0,1.0);
  slowPDF->SetParameter(1,LAr_slow_t);

  //Standard function for the TPB re-emission time constant.
  //This is model for 4 different time constants as modelled by the
  //Segreto paper:https://arxiv.org/abs/1411.4524
  TF1 *fastTPB = new TF1("fastTPB",DecayFunction,0,time_window,2);
  fastTPB->SetParameter(0,1.0);
  fastTPB->SetParameter(1,TPB_fast_t);
  TF1 *intTPB = new TF1("intTPB",DecayFunction,0,time_window,2);
  intTPB->SetParameter(0,1.0);
  intTPB->SetParameter(1,TPB_int_t);
  TF1 *longTPB = new TF1("longTPB",DecayFunction,0,time_window,2);
  longTPB->SetParameter(0,1.0);
  longTPB->SetParameter(1,TPB_long_t);
  TF1 *spuTPB = new TF1("spuTPB",DecayFunction,0,time_window,2);
  spuTPB->SetParameter(0,1.0);
  spuTPB->SetParameter(1,TPB_spu_t);

  //Set the SiPM correlated delayed pulse PDF
  //This is based on the following paper:
  //https://arxiv.org/abs/1703.06204
  TF1 *SiPMCDP = new TF1("SiPMCDP",SiPMcdpPDF,0,6000,6);
  SiPMCDP->SetParameter(5,0.005);
  SiPMCDP->SetParameter(0,1.0);
  SiPMCDP->SetParameter(1,22);
  SiPMCDP->SetParameter(2,10);
  SiPMCDP->SetParameter(3,0.05);
  SiPMCDP->SetParameter(4,100);
  TH1D *hCDP = SiPMcdpHist();
  int tw_bin = hCDP->GetXaxis()->FindBin(time_window-time_trigger); //tw: time window; h, hist: histogram
  double cdp_rate = hCDP->Integral(0,tw_bin)/1E9;

  //Set Up DarkNoise ranges
  double av_DN_num = (DN_rate*n_SiPM*SiPM_A/1E9)*time_window; //the /1E9 is to convert from s to ns

  //Set up time response function for the SiPM
  //This is a bit of a SiPM/DAQ related function
  //Smearing to the original "arrival time"
  TF1 *SiPMTimeResPDF = new TF1("SiPMTimeResPDF",SiPMTimeResolution,-3,3,3);
  SiPMTimeResPDF->SetParameter(0,1.0);
  SiPMTimeResPDF->SetParameter(1,0.0);
  SiPMTimeResPDF->SetParameter(2,SiPMTimeResWidth);

  //Define TGraphErrors for mean Prompt Ratio
  //of singlet and triplet based on YALE paper
  TGraphErrors *gNR = SingletToTripletYALE("NR");
  TGraphErrors *gNRs = SingletToTripletSCENE("NR");
  TGraphErrors *gER = SingletToTripletYALE("ER");

  //Set Simulation SEED
  rnd.SetSeed(seed);
  rndCDP.SetSeed(seed);

  //Define the Boundaries for reconstructed PSD
  double low_int_bound = time_trigger-4.0; //lower integration bound
  double high_int_bound = time_trigger+86.0; //higher integration bound

  //Check that the energy range allows leakage to be calculated properly.
  if (energy_min < 5 | energy_max > 56){cout << "Can only calculate leakage from 5 to 56 keV. See SingletToTripletYALE.c" << endl;}

  //Define Ntuple TTree
  //Write output
  TString num = Form("%ld",evt_max); //TString::Form formats a string
  TString name_pde = GetFloatAsString(pde);
  TString name_coll_eff = GetFloatAsString(coll_eff);
  TString nSeed = Form("S%d",seed);
  TString tpb = "";
  if (IsTPBon==1){tpb="TPBon";}
  else if (IsTPBon==0){tpb="TPBoff";}
  TString filename = "Data/SiPM_"+num+"_"+name_pde+"_"+name_coll_eff+"_"+nSeed+"_"+tpb+"_"+evt_type+".root";
  TFile *fout = TFile::Open(filename,"recreate");

  //defining variables to save in the TTree
  int ievt, n_scint_p, n_coll_p; //ievt: event number; n_scint_p: number of scintillation photons; n_coll_p: number of collected photons
  int badPSD = 0; //count of evts with PSD 0 or 1
  double tru_psd, erecoil, u_pr, rec_psd, residual, leak_energy, tot_good_nrg; // tru_psd: true PSD, erecoil: recoil energy, u_pr: true singlet to triplet ratio,
			 //rec_psd: recorded PSD (with noise & stuff added); residual: measure of goodness of rec_psd; leak_energy: the energy of ER events that
			 // leak into NR; tot_good_nrg: the energy of ER events that don't have rec_psd==0|1
  vector<double> pht_wl; //pulse vector? photon wavelength
  vector<double> pht_st; //pulse vector? phton emission time
  //defining a TTree
  TTree *SiPMmc = new TTree("SiPMmc","SiPM LAr Simulation");
  SiPMmc->Branch("ievt",&ievt);
  SiPMmc->Branch("tru_psd",&tru_psd);
  SiPMmc->Branch("rec_psd",&rec_psd);
  SiPMmc->Branch("residual",&residual);
  SiPMmc->Branch("leak_energy",&leak_energy);
  SiPMmc->Branch("tot_good_nrg",&tot_good_nrg);
  SiPMmc->Branch("u_pr",&u_pr);
  SiPMmc->Branch("erecoil",&erecoil);
  SiPMmc->Branch("n_scint_p",&n_scint_p);
  SiPMmc->Branch("n_coll_p",&n_coll_p);
  SiPMmc->Branch("badPSD",&badPSD);
  //gather my constants in a structure & save them in a branch
  char recoil_type;
  if (evt_type=="NR"){recoil_type='N';}
  else if (evt_type=="ER"){recoil_type='E';}
  constant_list this_run = MakeList(evt_max, energy_min, energy_max, pde, coll_eff, photo_yield, IsTPBon, recoil_type);
  SiPMmc->Branch("constants", &this_run, "evt_max/L:energy_min/D:energy_max/D:pde/D:coll_eff/D:photo_yield/D:tpb/I:evt_type/C");
  //Pietro wrote this and commented it out, I'm not sure why
  //SiPMmc->Branch("pht_st",&pht_st);
  //SiPMmc->Branch("pht_wl",&pht_wl);

  //--------------------------------------------------------------------//
  // Stage 2, Event by Event simulation                                //
  //--------------------------------------------------------------------//
  //Loop through all events
  for (int ievt=0; ievt<evt_max; ievt++)
    {

      //Clear Pulse Vector
      pht_wl.clear();
      pht_st.clear();

      //Print out progress in simulation
      if ((ievt%500)==0) {cout << "Event: " << ievt << " out of " << evt_max << endl;}

      //Generate a recoil energy for the event
      //Randomly distributed between energy_min and energy_max
      erecoil = rnd.Uniform(energy_min,energy_max);

      //Generate the true PSD for this event
      if (evt_type=="NR")
	{
	  u_pr = gNR->Eval(erecoil);
	  NRfunc->SetParameter(2,u_pr);
	  tru_psd = NRfunc->GetRandom()*0.01;
	}
      if (evt_type=="ER")
	{
	  u_pr = gER->Eval(erecoil);
	  ERfunc->SetParameter(2,u_pr);
	  tru_psd = ERfunc->GetRandom()*0.01;
	}


      //Get the number of scintillation photons produced for the
      //generated recoil energy
      n_scint_p = erecoil*photo_yield;

      //Account for detector collection efficiency
      //This can be changed for different experiments
      n_coll_p = n_scint_p*coll_eff;

      //Define Photons vectors
      //Plus define other variables
      int n_fast_p = n_coll_p*tru_psd; // number of prompt photon, based on gen PSD

      //Loop through all the generated photons
      //to assign a given wavelength and an emission time
      for (int ipht=0; ipht<n_coll_p; ipht++)
	{
	  //Define some basic variables for definition loop
	  double wl=0.0; //wavelength
	  double st=0.0; //emission time

	  //First off, assign a scintillation wavelength to the photon
	  //Based on the constructed emission spectrum
	  wl = LArEmission->GetRandom();

	  //Assign the emission time to each photon
	  //ratio of single to triplet is selected based
	  //on the simulated true PSD
	  double st_ratio = rnd.Uniform(0.0,1.0); //singlet triplet ratio?
	  if (st_ratio<=tru_psd){st = fastPDF->GetRandom();}
	  else if (st_ratio>tru_psd){st = slowPDF->GetRandom();}

	  //If TPB is applied to the detector
	  //Absorb and re-emit the photons based on
	  //Segreto's measurement
	  if (IsTPBon==1){
	    double tpb_emi_prob = rnd.Uniform(0.0,1.0);
	    if (tpb_emi_prob <= TPB_fast_r){st += fastTPB->GetRandom();}
	    if (tpb_emi_prob > TPB_fast_r && tpb_emi_prob <= (TPB_int_r+TPB_fast_r)){st += intTPB->GetRandom();}
	    if (tpb_emi_prob > (TPB_int_r+TPB_fast_r) && tpb_emi_prob <= (TPB_long_r+TPB_int_r+TPB_fast_r)){st += longTPB->GetRandom();}
	    if (tpb_emi_prob > (TPB_long_r+TPB_int_r+TPB_fast_r) && tpb_emi_prob <= 1.0){st += spuTPB->GetRandom();}
	  }

	  //if(ipht<=n_fast_p){st = fastPDF->GetRandom();}
	  //else{st = slowPDF->GetRandom();}

	  //Add the intrinsic resolution time for the SiPM devices
	  double st_offset = SiPMTimeResPDF->GetRandom(-3.0,3.0);
	  st += st_offset;

	  //Add DAQ Trigger time offset
	  st += time_trigger;

	  //Establish the survival probability for the generated photon
	  //based on the simulated emission wavelength and the selected
	  //SiPM PDE PDFs
	  double see_photon = rnd.Uniform(0.0,1.0);
	  if (see_photon<=pde)
	    {
	      pht_wl.push_back(wl);
	      pht_st.push_back(st);
	    }//see-loop
	}//end of ipht-loop

      //--------------------------------------------------------------------//
      // Stage 3, Add Correlated Noise and DN                               //
      //--------------------------------------------------------------------//
      //--------------------------------------------------------------------//
      // Add CDP pulses                                                     //
      //--------------------------------------------------------------------//
      for (int icdp=0; icdp<pht_st.size(); icdp++)
	{
	  double pp_time = pht_st.at(icdp);
	  double gen_p = rndCDP.Uniform(0.0,1.0);
	  double p0 = TMath::Poisson(0,cdp_rate);
	  double p1 = p0 + TMath::Poisson(1,cdp_rate);
	  double p2 = p1 + TMath::Poisson(2,cdp_rate);

	  int n=5;
	  double poisson=1.0;
	  while(1)
	    {
	      poisson = TMath::Poisson(n,cdp_rate);
	      if (gen_p<poisson || n==0){break;}
	      n--;
	    }//end while-loop

	  if (n==0){continue;}
	  else if (n>0)
	    {
	      for (int ap=0; ap<n; ap++)
		{

		  double ap_time = pp_time + hCDP->GetRandom();
		  pht_st.push_back(ap_time);
		}//end for loop
	    }//end if-else loop
	}//end of icdp-loop
      //--------------------------------------------------------------------//
      // Add Dark Noise                                                     //
      //--------------------------------------------------------------------//
      //Can have up to 5 dark noise pulses (prob. of >5 is 7e-22 with DN_rate=0.2; n_SiPM=30; SiPM_A=25; time_window=6000)
      int n_dn = 5;
      double p_dn = rndCDP.Uniform(0.0000,1.0000);
      double poisson_dn;

     //check whether there will be 5, 4, 3, 2, 1, or 0 DN photons
      while(1)
	{
	  poisson_dn = TMath::Poisson(n_dn,av_DN_num);
	  if (p_dn<poisson_dn || n_dn==0){break;}
	  n_dn--;
	}//end while-loop

      //if there are DN photons, add them to the data
      if (n_dn>0)
	{
	  for (int dn=0; dn<n_dn; dn++)
	    {
	      double dn_time = rnd.Uniform(0,time_window);
	      pht_st.push_back(dn_time);
	    }//end for-loop
	}//end if-else

      //Estimate the Reconstructed PSD after the SiPM WF Simulation
      double rec_prompt_count = 0.0;
      double rec_full_count = 0.0;
      for (int kk=0; kk<pht_st.size(); kk++)
	{
	  double single_pulse_time = pht_st.at(kk);
	  if (single_pulse_time>=low_int_bound && single_pulse_time<=high_int_bound){rec_prompt_count++;}
	  if (single_pulse_time>=low_int_bound){rec_full_count++;}
	}//end of kk-loop
      rec_psd = rec_prompt_count/rec_full_count;//estimate reconstructed PSD

      //the PSD residual is a measure of how well rec_psd reflects tru_psd
      residual = abs((tru_psd-rec_psd)/tru_psd);

      //If rec_psd==0|1, that means it's been reconstructed badly. We want to count how many events do that.
      if (rec_psd==0 | rec_psd==1) badPSD += 1;

      //leakage is the proportion of ER events that are miscategorized as NR because their PSD is above the average NR PSD
      //If rec_psd is >the average NR from YALE (and !=0|1), add the energy to leak_energy.
      if (evt_type=="ER" & (energy_min >= 5) & (energy_max <= 56)){ //check if the energy range is within the YALE energy range
	//find the correct energy bin
	int bin = 5;
	while (bin < (erecoil-1)){
	  bin ++;
	}
	//check if the event contributes to leakage
	long index = find(energy, energy+size, bin+0.5) - energy;
	if (rec_psd>nuclear[index] & rec_psd!=0 & rec_psd!=1) leak_energy = erecoil;
	else leak_energy = -1;
	//count the number of good events
	if (rec_psd!=0 & rec_psd!=1) tot_good_nrg = erecoil;
	else tot_good_nrg = -1;
      }

      //Fill TTree
      SiPMmc->Fill();

    }//end of ievt-loop

  //Write output
  SiPMmc->Write();
  fout->Close();

}//end void
