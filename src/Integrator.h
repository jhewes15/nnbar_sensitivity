#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "TMath.h"
#include "Math/WrappedFunction.h"
#include "Math/IntegratorMultiDim.h"
#include <iostream>
#include <vector>

// Need some global variables here

std::vector<double> globalParams;
double globalWidthMax;

// Integral functors

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
double factorial(double n)
{
  double n_factorial = 1;
  for (int i = n; i > 0; i--)
    n_factorial *= i;
  return n_factorial;
}

// 3D integral of probability distribution, with lifetime value fixed
double Integral3D(const double * x)
{
  // Get input variables
  double exposure   = x[0];
  double efficiency = x[1];
  double background = x[2];
  
  // Evaluate probability distributions for variables
  double p_exposure   = gaus(exposure  , globalParams[0], globalParams[1]);
  double p_efficiency = gaus(efficiency, globalParams[2], globalParams[3]);
  double p_background = gaus(background, globalParams[4], globalParams[5]);
  double p_width      = heaviside(globalWidthMax);
  
  // Get some other useful variables
  double n_factorial = factorial(globalParams[6]);
  double param = (exposure * efficiency * globalWidthMax) + background;
  
  // Evaluate Bayesian function
  double bayesian = (exp(-1 * param) * pow(param, globalParams[6])) / n_factorial;
  
  // Put it all together
  double everything = bayesian * p_exposure * p_efficiency * p_background * p_width;
  //std::cout << "exposure " << exposure << ", efficiency " << efficiency << ", background " << background << ", param " << param << ", bayesian " << bayesian << std::endl;
  //std::cout << "p_exposure " << p_exposure << ", p_efficiency " << p_efficiency << ", p_background " << p_background << ", p_width " << p_width << ", everything " << everything << std::endl;
  //std::cout << "globalWidthMax " << globalWidthMax << std::endl;
  return everything;
}


// 4D integral of probability distribution, including lifetime variable
double Integral4D(const double * x)
{
  // Get input variables
  double width      = x[0];
  double exposure   = x[1];
  double efficiency = x[2];
  double background = x[3];
  
  // Evaluate probability distributions for variables
  double p_exposure   = gaus(exposure  , globalParams[0], globalParams[1]);
  double p_efficiency = gaus(efficiency, globalParams[2], globalParams[3]);
  double p_background = gaus(background, globalParams[4], globalParams[5]);
  double p_width      = heaviside(width);
  
  // Get some other useful variables
  double n_factorial = factorial(globalParams[6]);
  double param = (exposure * efficiency * width) + background;
  
  // Evaluate Bayesian function
  double bayesian = (exp(-1 * param) * pow(param, globalParams[6])) / n_factorial;
  
  // Put it all together
  double everything = bayesian * p_exposure * p_efficiency * p_background * p_width;
  return everything;
}

// Integrator class - an object to do all the lifetime limit calculation for nnbar
class Integrator{
  
public:
  
  // Default constructor & destructor
  Integrator();
  ~Integrator(){}
  
  // Setter/getter functions
  void SetProcess(int process) { _process = process; };
  void SetParams(std::vector<double> params);
  void SetRate(double n_obs) { CheckParams(); _finalParams[6] = n_obs; };
  void CheckParams();
  void SetMass(double mass) { _mass = mass; };
  void SetDensity(double density) { _density = density; };
  std::vector<double> GetParams() { CheckParams(); return _finalParams; };
  double GetWidthMax() { return _widthMax; };
  double GetLifetime90CL() { return (1 / _width); };
  
  // Integrator functions
  void FindWidthRange();
  
  // Limit calculation
  double Limit();
  
  // Output info
  void PrintParams();
  
private:
  
  // Member variables
  bool _verbose;
  int _process;
  
  // Sensitivity parameters
  std::vector<double> _inputParams;
  std::vector<double> _finalParams;
  std::vector<double> _intervals;
  double _widthMax;
  double _mass;
  double _density;
  double _width;
};

#endif
