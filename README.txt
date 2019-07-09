//-----------------------------//
//                             //
// Pietro Giampa, TRIUMF, 2018 //
//-----------------------------//

//
// CODE OPERATION
//

1) Run the Simulator with the following line:

   root -l 'RunMC.c(TPN on/off, Random SEED, "NR"/"ER")'

2) Graph the simulation data
   Graphs with either ER or NR:
   root -l 'ReadOutput1.c("Data/<filename>")'
   
   Graphs with both ER and NR:
   root -l 'ReadOutput2.c("Data/<ER_filename>", "Data/<NR_filename>")'

//
// Filename
//
   SiPM_"+number of events+"_"+PDE+"_"+collection efficiency+"_"+Seed+"_"+TPBon/TPBoff+"_"+NR/ER+".root"

//---------------------------//
// AVAILABLE FUNCTIONALITIES //
//---------------------------//

0) constants.c
1) DecayFunction.c
2) LArEmissionSpectrum.c
3) SiPMTimeResolution.c
4) SiPMcdpHist.c
5) SiPMcdpPDF.c
6) SingletToTripletFunction.c
7) SingletToTripletSCENE.c
8) SingletToTripletYALE.c

   
