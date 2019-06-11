//Define all the variables

//https://arxiv.org/abs/1403.0525
double photo_yield = 40.0; //LAr photo yield [photons/keV]
double LAr_fast_t = 7.0; //LAr fast scintillation time const [ns]
double LAr_slow_t = 1600.0; //LAr slow scintillation time const [ns]

//https://arxiv.org/abs/1411.4524
double TPB_fast_t = 10.0; //TPB Fast time constant [ns]
double TPB_int_t = 49.0; //TPB intermediate time constant [ns]
double TPB_long_t = 3550.0; //TPB long time constant [ns]
double TPB_spu_t = 309.0; //TPB spurious time constant [ns]
double TPB_fast_r = 0.6; //abundance
double TPB_int_r = 0.3; //abundance
double TPB_long_r = 0.08; //abundance
double TPB_spu_r = 0.02; //abundance (abundances should add up to 1)

//https://arxiv.org/abs/1903.03663
double pde = 0.125; //PDE for the Hamamatsu VUV4 SIPM at 189 nm
double DN_rate = 0.2; //Hz/mm^2, for the Hamamatsu VUV4

TRandom3 rnd;
TRandom3 rndCDP;

// set by user
double coll_eff = 0.2; //detector collection efficiency [%] - I think this is light coverage/area coverage of photodetectors
double overvoltage = 4.0; //Set the SiPM overvoltage [V]
double time_window = 6000.0; //DAQ recording window [ns]
double time_trigger = 2000.0; //DAQ Trigger time [ns]
double time_full_window = time_window+time_trigger; //DAQ full window [ns]
int n_SiPM = 30; //number of SiPMs in the experiment
double SiPM_A = 25; //area of one SiPM in mm2
double energy_min = 5.0; //minimum event energy in keV (5 is the minimum our code can handle)
double energy_max = 20.0; //maximum event energy in keV
double SiPMTimeResWidth = 0.5; //std deviation of the gaussian function blurring the SiPM time measurements

