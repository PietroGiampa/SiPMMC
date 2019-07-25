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
   root -l 'ReadOutput.c("Data/<filename>")'
   
   Graphs with both ER and NR:
   root -l 'ReadOutput.c("Data/<ER_filename>", "Data/<NR_filename>")'

   Graphs with different constants
   root -l
   TString files[len] = {"Data/<file1>","Data/<file2>"...}
   .L ConstCompare.c
   ConstCompare(len,files)

//
// Filename
//
   SiPM_"+number of events+"_"+PDE+"_"+collection efficiency+"_"+Seed+"_"+TPBon/TPBoff+"_"+NR/ER+".root"

//---------------------------//
// AVAILABLE FUNCTIONALITIES //
//---------------------------//

0) abbreviations.txt
1) constants.c
2) ConstCompare.c
3) DecayFunction.c
4) GetFloatAsString.c
5) LArEmissionSpectrum.c
6) MakeList.c
7) nr50.c
8) ReadOutput.c
9) RunMC.c
10) SiPMTimeResolution.c
11) SiPMcdpHist.c
12) SiPMcdpPDF.c
13) SingletToTripletFunction.c
14) SingletToTripletSCENE.c
15) SingletToTripletYALE.c

   
