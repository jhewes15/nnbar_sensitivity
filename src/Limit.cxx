
#include "limit.h"

// Debug output
bool debug = false;

//////////////////////////////////////////
//                                      //
//              PARAMETERS              //
//                                      //
//////////////////////////////////////////


// Lifetime defined here so it's always in scope

double lifetime = 0;


// super-k parameters

double efficiency_mean  = 0.121;
double efficiency_sigma = 0.02771;
double exposure_mean    = 243.0e32;
double exposure_sigma   = 7.35e32;
double background_mean  = 24.1;
double background_sigma = 5.712;
int n = 24;


// Main function

void Limit(int i_lifetime, TString run_name, bool params = false, double u_efficiency_mean = 0, double u_efficiency_sigma = 0, double u_exposure_mean = 0, double u_exposure_sigma = 0, double u_background_mean = 0, double u_background_sigma = 0, double u_n = 0)
{
  // Input custom parameters if enabled
  if (params)
  {
    efficiency_mean  = u_efficiency_mean;
    efficiency_sigma = u_efficiency_sigma;
    exposure_mean    = u_exposure_mean;
    exposure_sigma   = u_exposure_sigma;
    background_mean  = u_background_mean;
    background_sigma = u_background_sigma;
    n = u_n;
  }

  // Spit out the variables used
  std::cout << "Parameters used:" << std::endl;
  std::cout << "  Efficiency mean:  " << efficiency_mean  << std::endl;
  std::cout << "  Efficiency sigma: " << efficiency_sigma << std::endl;
  std::cout << "  Exposure mean:    " << exposure_mean    << std::endl;
  std::cout << "  Exposure sigma:   " << exposure_sigma   << std::endl;
  std::cout << "  Background mean:  " << background_mean  << std::endl;
  std::cout << "  Background sigma: " << background_sigma << std::endl;
  std::cout << "  Number of events: " << n                << std::endl;

  // Set up 4D integrator
  ROOT::Math::Functor f4D(integral4D, 4);
  ROOT::Math::IntegratorMultiDim ig4D(ROOT::Math::IntegrationMultiDim::kADAPTIVE);
  ROOT::Math::IntegratorMultiDim ig4D(f4D);
  ig4D.SetRelTolerance(0.01);
  
  // Set limits for 4D integration
  double xmin4D[4];
  double xmax4D[4];
  
  // Do a 4D integral using many multiple bounds
  
  double interval_lifetime   = 1.e-34;
  double interval_efficiency = 0.01;
  double interval_exposure   = 5e32;
  double interval_background = 0.5;
  
  double integral = 0;
  double lifetime = (i_lifetime + 0.5) * interval_lifetime;
  
  // Split three remaining integration dimensions into 100 small integrals
  for (int i_efficiency = 0; i_efficiency < 100; i_efficiency++) {
    for (int i_exposure = 0; i_exposure < 100; i_exposure++) {
      for (int i_background = 0; i_background < 100; i_background++) {
        
        // Lifetime integral limits
        xmin4D[0] = i_lifetime * interval_lifetime;
        xmax4D[0] = (i_lifetime + 1) * interval_lifetime;
        
        // Efficiency integral limits
        xmin4D[1] = i_efficiency * interval_efficiency;
        xmax4D[1] = (i_efficiency + 1) * interval_efficiency;
        
        // Exposure integral limits
        xmin4D[2] = i_exposure * interval_exposure;
        xmax4D[2] = (i_exposure + 1) * interval_exposure;
        
        // Background integral limits
        xmin4D[3] = i_background * interval_background;
        xmax4D[3] = (i_background + 1) * interval_background;
        
        integral += ig4D.Integral(xmin4D, xmax4D);
      }
    }
    std::cout << i_efficiency << "% done" << std::endl;
  }
  
  std::cout << "Made it through all the integral loops. All that's left is writing to file." << std::endl;

  // Now spit the lifetime & integral value into an output file for analysis.
  ofstream outfile;
  TString outfilename = Form("%s/integral_%s_%i.txt", getenv("CONDOR_DIR_OUT"), run_name.Data(), i_lifetime);
  std::cout << "Outfilename is " << outfilename.Data() << std::endl;
  outfile.open(outfilename.Data());
  outfile << lifetime << " " << integral;
  outfile.close();
}
