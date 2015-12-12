#include "TMath.h"
#include "Math/WrappedFunction.h"
#include "Math/IntegratorMultiDim.h"
#include <iostream>
#include <fstream>
#include <string>

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
void Limit(int i_lifetime, TString run_name, bool params = false, double u_efficiency_mean = 0, double u_efficiency_sigma = 0, double u_exposure_mean = 0, double u_exposure_sigma = 0, double u_background_mean = 0, double u_background_sigma = 0, double u_n = 0);

