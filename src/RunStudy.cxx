#include <string>
#include "Integrator.h"

//
// summary of the variables and their units:
//
// exposure is years
// background is events per year
// efficiency is just a fraction
//
// all uncertainties are fractional
//

void RunStudy(int i_lifetime, std::string run_name, double exposure, double uncertainty_exposure, double efficiency, double uncertainty_efficiency, double background, double uncertainty_background) {
  
  int n_studies = 10;
  
  
  // define constants
  double mass    = 40;           // kt
  double density = 3.3164892e32; // neutrons per kt
  
  // convert values of input parameters
  
  double exp       = mass * density * exposure;
  double sigma_exp = uncertainty_exposure * exp;
  double eff       = efficiency;
  double sigma_eff = uncertainty_efficiency * eff;
  double bkg       = background * mass * exposure;
  double sigma_bkg = uncertainty_background * bkg;
  double n_obs     = bkg;

  std::cout << "RunStudy.cxx sensitivity parameters:" << std::endl;
  std::cout << "  Exp     - " << exp       << std::endl;
  std::cout << "  Exp_sig - " << sigma_exp << std::endl;
  std::cout << "  Eff     - " << eff       << std::endl;
  std::cout << "  Eff_sig - " << sigma_eff << std::endl;
  std::cout << "  Bkg     - " << bkg       << std::endl;
  std::cout << "  Bkg_sig - " << sigma_bkg << std::endl;
  std::cout << "  N_obs   - " << n_obs     << std::endl;

  for (int i = 0; i < n_studies; i++) {
    int process = (n_studies * i_lifetime) + i;
    LimitTest(process, run_name.c_str(), true, exp, sigma_exp, eff, sigma_eff, bkg, sigma_bkg, n_obs);
  }
}
