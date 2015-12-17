#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "TMath.h"
#include "Math/WrappedFunction.h"
#include "Math/IntegratorMultiDim.h"
#include <iostream>
#include <fstream>
#include <string>

// Debug output
bool debug = false;

// lifetime
double lifetime = 0;

// super-k parameters

double efficiency_mean  = 0.121;
double efficiency_sigma = 0.02771;
double exposure_mean    = 243.0e32;
double exposure_sigma   = 7.35e32;
double background_mean  = 24.1;
double background_sigma = 5.712;
int n = 24;


// Gaussian distribution function

double gaus(double param, double mean, double sigma)
{
  double gaussian = (1 / (sqrt(2 * TMath::Pi()) * sigma)) * exp(-1 * pow(param - mean, 2) / (2 * pow(sigma, 2)));
  return gaussian;
}


// Heaviside distribution function

double heaviside(double param)
{
  if (param > 0)
    return 1;
  else if (param < 0)
    return 0;
  else if (param == 0)
    return 0.5;
  else
  {
    std::cout << "Error processing Heaviside function; lifetime param has no value!" << std::endl;
    exit(1);
  }
}


// Factorial function

double factorial(int n)
{
  double n_factorial = 1;
  for (int i = n; i > 0; i--)
    n_factorial *= i;
  return n_factorial;
}


// Find closer variable

bool findClosestVariable(double target, double test, double current)
{
  double current_dif = fabs(target - current);
  double new_dif     = fabs(target - test);
  
  if (debug)
    std::cout << "Current dif = " << current_dif << ", new dif = " << new_dif << std::endl;
  
  if (new_dif < current_dif)
    return 1;
  else
    return 0;
}



// 3D integral of probability distribution, with lifetime value fixed

double integral3D(const double * x)
{
  // Get input variables
  double efficiency = x[0];
  double exposure   = x[1];
  double background = x[2];
  
  if (debug)
  {
    std::cout << "\n\n----------------------------------" << std::endl;
    std::cout << "--------- EVENT SUMMARY ----------" << std::endl;
    std::cout << "----------------------------------" << std::endl;
  }
  
  // Evaluate probability distributions for variables
  double p_efficiency = gaus(efficiency, efficiency_mean, efficiency_sigma);
  double p_exposure   = gaus(exposure  , exposure_mean,   exposure_sigma  );
  double p_background = gaus(background, background_mean, background_sigma);
  double p_lifetime   = heaviside(lifetime);
  
  if (debug)
  {
    std::cout << "\nEfficiency   -   value: " << efficiency << ", probability: " << p_efficiency << std::endl;
    std::cout << "Exposure     -   value: "   << exposure   << ", probability: " << p_exposure   << std::endl;
    std::cout << "Background   -   value: "   << background << ", probability: " << p_background << std::endl;
    std::cout << "Lifetime     -   value: "   << lifetime   << ", probability: " << p_lifetime   << std::endl;
  }
  
  // Get some other useful variables
  double n_factorial = factorial(n);
  double param = (efficiency * exposure * lifetime) + background;
  
  // Evaluate Bayesian function
  double bayesian = (exp(-1 * param) * pow(param, n)) / n_factorial;
  
  if (debug)
  {
    std::cout << "\nBAYESIAN DISTRIBUTION" << std::endl;
    std::cout << "Parameter:       " << param           << std::endl;
    std::cout << "exp(-1 * param): " << exp(-1 * param) << std::endl;
    std::cout << "pow(param, n):   " << pow(param, n)   << std::endl;
    std::cout << "n_factorial:     " << n_factorial     << std::endl;
    std::cout << "Final value:     " << bayesian        << std::endl;
  }
  
  // Put it all together
  double everything = bayesian * p_efficiency * p_exposure * p_background * p_lifetime;
  
  if (debug)
  {
    std::cout << "\nEVERYTHING"          << std::endl;
    std::cout << "Value: " << everything << std::endl;
  }
  
  return everything;
}


// 4D integral of probability distribution, including lifetime variable

double integral4D(const double * x)
{
  // Get input variables
  double lifetime   = x[0];
  double efficiency = x[1];
  double exposure   = x[2];
  double background = x[3];
  
  if (debug)
  {
    std::cout << "\n\n----------------------------------" << std::endl;
    std::cout << "--------- EVENT SUMMARY ----------" << std::endl;
    std::cout << "----------------------------------" << std::endl;
  }
  
  // Evaluate probability distributions for variables
  double p_efficiency = gaus(efficiency, efficiency_mean, efficiency_sigma);
  double p_exposure   = gaus(exposure  , exposure_mean,   exposure_sigma  );
  double p_background = gaus(background, background_mean, background_sigma);
  double p_lifetime   = heaviside(lifetime);
  
  if (debug)
  {
    std::cout << "\nEfficiency   -   value: " << x[0] << ", probability: " << p_efficiency << std::endl;
    std::cout << "Exposure     -   value: "   << x[1] << ", probability: " << p_exposure   << std::endl;
    std::cout << "Background   -   value: "   << x[2] << ", probability: " << p_background << std::endl;
    std::cout << "Lifetime     -   value: "   << x[3] << ", probability: " << p_lifetime   << std::endl;
  }
  
  // Get some other useful variables
  double n_factorial = factorial(n);
  double param = (efficiency * exposure * lifetime) + background;
  
  // Evaluate Bayesian function
  double bayesian = (exp(-1 * param) * pow(param, n)) / n_factorial;
  
  if (debug)
  {
    std::cout << "\nBAYESIAN DISTRIBUTION" << std::endl;
    std::cout << "Parameter:       " << param           << std::endl;
    std::cout << "exp(-1 * param): " << exp(-1 * param) << std::endl;
    std::cout << "pow(param, n):   " << pow(param, n)   << std::endl;
    std::cout << "n_factorial:     " << n_factorial     << std::endl;
    std::cout << "Final value:     " << bayesian        << std::endl;
  }
  
  // Put it all together
  double everything = bayesian * p_efficiency * p_exposure * p_background * p_lifetime;
  
  if (debug)
  {
    std::cout << "\nEVERYTHING"          << std::endl;
    std::cout << "Value: " << everything << std::endl;
  }
  
  return everything;
}

// Main function

void Limit(int i_lifetime, TString run_name, bool params = false, double u_exposure_mean = 0, double u_exposure_sigma = 0, double u_efficiency_mean = 0, double u_efficiency_sigma = 0, double u_background_mean = 0, double u_background_sigma = 0, double u_n = 0)
{
  // Input custom parameters if enabled
  if (params)
  {
    exposure_mean    = u_exposure_mean;
    exposure_sigma   = u_exposure_sigma;
    efficiency_mean  = u_efficiency_mean;
    efficiency_sigma = u_efficiency_sigma;
    background_mean  = u_background_mean;
    background_sigma = u_background_sigma;
    n = u_n;
  }
  
  // Spit out the variables used
  std::cout << "Limit parameters used:" << std::endl;
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

void WidthRange(double val_exp, double sig_exp, double val_eff, double sig_eff, double val_bkg, double sig_bkg, double n)
{
  // Spit out the variables used
  std::cout << "WidthRange parameters used:"     << std::endl;
  std::cout << "  Exposure mean:    " << val_exp << std::endl;
  std::cout << "  Exposure sigma:   " << sig_exp << std::endl;
  std::cout << "  Efficiency mean:  " << val_eff << std::endl;
  std::cout << "  Efficiency sigma: " << sig_eff << std::endl;
  std::cout << "  Background mean:  " << val_bkg << std::endl;
  std::cout << "  Background sigma: " << sig_bkg << std::endl;
  std::cout << "  Number of events: " << n       << std::endl;
  
  // Set up 3D integrator
  ROOT::Math::Functor f3D(integral3D, 3);
  ROOT::Math::IntegratorMultiDim ig3D(ROOT::Math::IntegrationMultiDim::kADAPTIVE);
  ROOT::Math::IntegratorMultiDim ig3D(f3D);
  ig3D.SetRelTolerance(0.01);
  
  // Figure out what scale the integral should be set to
  double xmin3D[3];
  double xmax3D[3];
  
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

#endif