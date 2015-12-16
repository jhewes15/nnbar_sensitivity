#include <string>
#include "Limit.h"

//
// summary of the variables and their units:
//
// exposure is years
// background is events per year
// efficiency is just a fraction
//
// all uncertainties are fractional
//

void RunStudy.cxx(int i_lifetime, string run_name, double exposure, double uncertainty_exposure, double background, double uncertainty_background, double efficiency, double uncertainty_efficiency) {
  
  int n_studies = 10;
  
  
  // define constants
  double mass = 40;         // kt
  double density = 3.316e29 // neutrons per kt
  
  // convert values of input parameters
  
  double exp       = mass * density * exposure;
  double sigma_exp = uncertainty_exposure * exp;
  double eff       = efficiency;
  double sigma_eff = uncertainty_efficiency * eff;
  double bkg       = background * mass * exposure;
  double sigma_bkg = uncertainty_background * bkg;
  double n_obs     = bkg;

  for (int i = 0; i < n_studies; i++) {
    int process = (n_studies * i_lifetime) + i;
    Limit(process, run_name.c_str(), true, eff, sigma_eff, exp, sigma_exp, bkg, sigma_bkg, n_obs)

  }
}
