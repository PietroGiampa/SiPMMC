#include "SingletToTripletYALE.c"
#include "math.h"

double exp_fit(double* x, double* pars){
	return pars[0]*exp(pars[1]*x[0]+pars[2]) + pars[3];
}


void nr50(){
	TGraphErrors *nr50_data = new TGraphErrors(35,energy,nuclear,energyR,nuclearR);
	nr50_data->Draw();

	TF1 *fit = new TF1("fit",exp_fit,5.5,55.5,4);
	fit->SetParameter(0,-1);
	fit->SetParameter(1,-1);
	fit->SetParameter(2,0);
	fit->SetParameter(3,1);

	auto fitPtr = nr50_data->Fit(fit,"S");
	fitPtr->Print();

	double parameters[4] = {fit->GetParameter(0),fit->GetParameter(1),fit->GetParameter(2),fit->GetParameter(3)};

	return parameters;

}

//the above code returns the following parameters
double pars[4] = {-4.73167,-0.167845,-2.67172,0.700374};
