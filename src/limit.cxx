#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstring>
#include <cmath>
#include <ctime>

#include "cubature.h"

bool verbose = false;

double global_eff = 0;
double global_bkg = 0;

double poisson_threshold = 10;

// Utility function
void print(std::vector<double> params)
{
  std::cout << std::endl;
  std::cout << "~~~~~~~~~~~~~ INPUT PARAMETERS ~~~~~~~~~~~~~" << std::endl;
  std::cout << std::left << std::setw(20) << "Exposure"         << std::right << std::setw(20) << params[0] << std::endl;
  std::cout << std::left << std::setw(20) << "Exposure (sig)"   << std::right << std::setw(20) << params[1] << std::endl;
  std::cout << std::left << std::setw(20) << "Efficiency"       << std::right << std::setw(20) << params[2] << std::endl;
  std::cout << std::left << std::setw(20) << "Efficiency (sig)" << std::right << std::setw(20) << params[3] << std::endl;
  std::cout << std::left << std::setw(20) << "Background"       << std::right << std::setw(20) << params[4] << std::endl;
  std::cout << std::left << std::setw(20) << "Background (sig)" << std::right << std::setw(20) << params[5] << std::endl;
  std::cout << std::left << std::setw(20) << "Event rate"       << std::right << std::setw(20) << params[6] << std::endl;
  std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
  std::cout << std::endl;
}

// Gaussian distribution function
double gaus(double param, double mean, double sigma)
{
  double gaussian = (1 / (sqrt(2 * M_PI) * sigma)) * exp(-1 * pow(param - mean, 2) / (2 * pow(sigma, 2)));
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
    n_factorial *= (double) i;
  return n_factorial;
}

int f_combined(unsigned ndim, const double * x, void * params, unsigned fdim, double * fval)
{
  // Get input variables
  double width      = x[0];
  double exposure   = x[1];
  double efficiency = x[2];
  double background = x[3];

  std::vector<double> integralParams = *((std::vector<double> *) params);

  global_eff = integralParams[2];
  global_bkg = integralParams[4];

  if (verbose)
    std::cout << "--------------------------------------------" << std::endl;

  // Evaluate probability distributions for variables
  if (verbose) {
    std::cout << std::left << std::setw(20) << "width" << std::right << std::setw(20) << width << std::endl;
    std::cout << std::left << std::setw(20) << "exposure" << std::right << std::setw(20) << exposure
        << " (" << integralParams[0] << "->" << integralParams[1] << ")" << std::endl;
    std::cout << std::left << std::setw(20) << "efficiency" << std::right << std::setw(20) << efficiency
        << " (" << integralParams[2] << "->" << integralParams[3] << ")" << std::endl;
    std::cout << std::left << std::setw(20) << "background" << std::right << std::setw(20) << background
        << " (" << integralParams[4] << "->" << integralParams[5] << ")" << std::endl;
  }

  double p_exposure   = gaus(exposure  , integralParams[0], integralParams[1]);
  double p_efficiency = gaus(efficiency, integralParams[2], integralParams[3]);
  double p_background = gaus(background, integralParams[4], integralParams[5]);
  double p_width      = heaviside(width);

  if (verbose) {
    std::cout << std::left << std::setw(20) << "p_exposure" << std::right << std::setw(20) << p_exposure << std::endl;
    std::cout << std::left << std::setw(20) << "p_efficiency" << std::right << std::setw(20) << p_efficiency << std::endl;
    std::cout << std::left << std::setw(20) << "p_background" << std::right << std::setw(20) << p_background << std::endl;
    std::cout << std::left << std::setw(20) << "p_width" << std::right << std::setw(20) << p_width << std::endl;
  }

  if (std::isnan(p_efficiency)) {
    if (verbose) std::cout << "Integral failed due to nan p_efficiency." << std::endl;
    return 1;
  }
  else if (std::isnan(p_background)) {
    if (verbose) std::cout << "Integral failed due to nan p_background." << std::endl;
    return 1;
  }

  // Get some other useful variables
  double n_factorial = factorial(integralParams[6]);
  if (verbose)
    std::cout << std::left << std::setw(20) << "n_factorial" << std::right << std::setw(20) << n_factorial << std::endl;
  if (std::isinf(n_factorial)) {
    if (verbose)
      std::cout << "Integral failed due to infinite n_factorial." << std::endl;
    return 1;
  }

  double param = (exposure * efficiency * width) + background;
  if (verbose)
    std::cout << std::left << std::setw(20) << "param" << std::right << std::setw(20) << param << std::endl;

  // Evaluate Bayesian function
  double var;
  if (param < 10) var = (exp(-1 * param) * pow(param, integralParams[6])) / n_factorial;
  else var = gaus(integralParams[6],param,sqrt(param));
  if (verbose)
    std::cout << std::left << std::setw(20) << "variable" << std::right << std::setw(20) << var << std::endl;
  if (std::isnan(var)) {
    if (verbose)
      std::cout << "Integral failed due to nan bayesian." << std::endl;
    return 1;
  }
  else if (std::isinf(var)) {
    if (verbose)
      std::cout << "Integral failed due to infinite bayesian." << std::endl;
    return 1;
  }

  // Put it all together
  double everything = var * p_exposure * p_efficiency * p_background * p_width;
  if (verbose)
    std::cout << std::left << std::setw(20) << "everything" << std::right << std::setw(20) << everything << std::endl;
  fval[0] = everything;

  return 0;
}

// 4D integral of poisson probability distribution, including lifetime variable
int f_poisson(unsigned ndim, const double * x, void * params, unsigned fdim, double * fval)
{
  // Get input variables
  double width      = x[0];
  double exposure   = x[1];
  double efficiency = x[2];
  double background = x[3];

  std::vector<double> integralParams = *((std::vector<double> *) params);

  global_eff = integralParams[2];
  global_bkg = integralParams[4];

  if (verbose)
    std::cout << "--------------------------------------------" << std::endl;

  // Evaluate probability distributions for variables
  if (verbose) {
    std::cout << std::left << std::setw(20) << "width" << std::right << std::setw(20) << width << std::endl;
    std::cout << std::left << std::setw(20) << "exposure" << std::right << std::setw(20) << exposure
        << " (" << integralParams[0] << "->" << integralParams[1] << ")" << std::endl;
    std::cout << std::left << std::setw(20) << "efficiency" << std::right << std::setw(20) << efficiency
        << " (" << integralParams[2] << "->" << integralParams[3] << ")" << std::endl;
    std::cout << std::left << std::setw(20) << "background" << std::right << std::setw(20) << background
        << " (" << integralParams[4] << "->" << integralParams[5] << ")" << std::endl;
  }

  double p_exposure   = gaus(exposure  , integralParams[0], integralParams[1]);
  double p_efficiency = gaus(efficiency, integralParams[2], integralParams[3]);
  double p_background = gaus(background, integralParams[4], integralParams[5]);
  double p_width      = heaviside(width);

  if (verbose) {
    std::cout << std::left << std::setw(20) << "p_exposure" << std::right << std::setw(20) << p_exposure << std::endl;
    std::cout << std::left << std::setw(20) << "p_efficiency" << std::right << std::setw(20) << p_efficiency << std::endl;
    std::cout << std::left << std::setw(20) << "p_background" << std::right << std::setw(20) << p_background << std::endl;
    std::cout << std::left << std::setw(20) << "p_width" << std::right << std::setw(20) << p_width << std::endl;
  }

  if (std::isnan(p_efficiency)) {
    if (verbose) std::cout << "Integral failed due to nan p_efficiency." << std::endl;
    return 1;
  }
  else if (std::isnan(p_background)) {
    if (verbose) std::cout << "Integral failed due to nan p_background." << std::endl;
    return 1;
  }

  // Get some other useful variables
  double n_factorial = factorial(integralParams[6]);
  if (verbose)
    std::cout << std::left << std::setw(20) << "n_factorial" << std::right << std::setw(20) << n_factorial << std::endl;
  if (std::isinf(n_factorial)) {
    if (verbose)
      std::cout << "Integral failed due to infinite n_factorial." << std::endl;
    return 1;
  }

  double param = (exposure * efficiency * width) + background;
  if (verbose)
    std::cout << std::left << std::setw(20) << "param" << std::right << std::setw(20) << param << std::endl;

  // Evaluate Bayesian function
  double bayesian = (exp(-1 * param) * pow(param, integralParams[6])) / n_factorial;
  if (verbose)
    std::cout << std::left << std::setw(20) << "bayesian" << std::right << std::setw(20) << bayesian << std::endl;
  if (std::isnan(bayesian)) {
    if (verbose)
      std::cout << "Integral failed due to nan bayesian." << std::endl;
    return 1;
  }
  else if (std::isinf(bayesian)) {
    if (verbose)
      std::cout << "Integral failed due to infinite bayesian." << std::endl;
    return 1;
  }

  // Put it all together
  double everything = bayesian * p_exposure * p_efficiency * p_background * p_width;
  if (verbose)
    std::cout << std::left << std::setw(20) << "everything" << std::right << std::setw(20) << everything << std::endl;
  fval[0] = everything;

  return 0;
}

// 4D integral of poisson probability distribution, including lifetime variable
int f_gaussian(unsigned ndim, const double * x, void * params, unsigned fdim, double * fval)
{
  // Get input variables
  double width      = x[0];
  double exposure   = x[1];
  double efficiency = x[2];
  double background = x[3];

  std::vector<double> integralParams = *((std::vector<double> *) params);

  global_eff = integralParams[2];
  global_bkg = integralParams[4];

  if (verbose)
    std::cout << "--------------------------------------------" << std::endl;

  // Evaluate probability distributions for variables
  if (verbose) {
    std::cout << std::left << std::setw(20) << "width" << std::right << std::setw(20) << width << std::endl;
    std::cout << std::left << std::setw(20) << "exposure" << std::right << std::setw(20) << exposure
        << " (" << integralParams[0] << "->" << integralParams[1] << ")" << std::endl;
    std::cout << std::left << std::setw(20) << "efficiency" << std::right << std::setw(20) << efficiency
        << " (" << integralParams[2] << "->" << integralParams[3] << ")" << std::endl;
    std::cout << std::left << std::setw(20) << "background" << std::right << std::setw(20) << background
        << " (" << integralParams[4] << "->" << integralParams[5] << ")" << std::endl;
  }

  double p_exposure   = gaus(exposure  , integralParams[0], integralParams[1]);
  double p_efficiency = gaus(efficiency, integralParams[2], integralParams[3]);
  double p_background = gaus(background, integralParams[4], integralParams[5]);
  double p_width      = heaviside(width);

  if (verbose) {
    std::cout << std::left << std::setw(20) << "p_exposure" << std::right << std::setw(20) << p_exposure << std::endl;
    std::cout << std::left << std::setw(20) << "p_efficiency" << std::right << std::setw(20) << p_efficiency << std::endl;
    std::cout << std::left << std::setw(20) << "p_background" << std::right << std::setw(20) << p_background << std::endl;
    std::cout << std::left << std::setw(20) << "p_width" << std::right << std::setw(20) << p_width << std::endl;
  }

  if (std::isnan(p_efficiency)) {
    if (verbose) std::cout << "Integral failed due to nan p_efficiency." << std::endl;
    return 1;
  }
  else if (std::isnan(p_background)) {
    if (verbose) std::cout << "Integral failed due to nan p_background." << std::endl;
    return 1;
  }

  double param = (exposure * efficiency * width) + background;
  if (verbose)
    std::cout << std::left << std::setw(20) << "param" << std::right << std::setw(20) << param << std::endl;

  // Evaluate Bayesian function
  double gaussian = gaus(integralParams[6],param,sqrt(param));
  if (verbose)
    std::cout << std::left << std::setw(20) << "gaussian" << std::right << std::setw(20) << gaussian << std::endl;

  // Put it all together
  double everything = gaussian * p_exposure * p_efficiency * p_background * p_width;
  if (verbose)
    std::cout << std::left << std::setw(20) << "everything" << std::right << std::setw(20) << everything << std::endl;
  fval[0] = everything;

  return 0;
}

int main(int argc, char * argv[]) {

  clock_t start = clock();

  bool use_uboone = false;
  if ((argc == 10 && strcmp(argv[9],"0")!=0)||(argc == 9 && strcmp(argv[8],"0")!=0)) use_uboone = true;

  double mass; // kt
  if (use_uboone) mass = 0.089;
  else mass = 40;
  double density = 3.3164892e32; // neutrons / kt 
  
  // dune mass / density
  //double mass    = 40;           // kt
  //double density = 3.3164892e32; // neutrons / kt

  // microboone mass / density
  //double mass    = 0.089;        // kt
  //double density = 3.3164892e32; // neutrons/kt
  
  // adding in super-k parameters for debugging purposes
  //double mass    = 22.5;          // kt
  //double density = 2.6742369e+32; // neutrons / kt

  global_eff = 0;
  global_bkg = 0;

  // check command line arguments  
  if (argc < 9 || argc > 10) {
    std::cerr << "Usage: " << argv[0] << " [run_name] [params]" << std::endl;
    exit(1);
  }
 
  // parse the input parameters
  std::string run_name = argv[1];
  std::vector<double> initialParams;
  initialParams.reserve(6);
  for (int i = 2; i < 8; i++)
    initialParams.push_back(atof(argv[i]));

  // Convert input values into their final form
  std::vector<double> finalParams;
  finalParams.reserve(7);
  finalParams.push_back(initialParams[0] * mass * density);
  finalParams.push_back(initialParams[1] * finalParams[0]);
  finalParams.push_back(initialParams[2]);
  finalParams.push_back(initialParams[3] * finalParams[2]);
  finalParams.push_back(initialParams[4] * mass * initialParams[0]);
  finalParams.push_back(initialParams[5] * finalParams[4]);
  if (argc == 10)
    finalParams.push_back(atof(argv[8]));
  else
    finalParams.push_back(finalParams[4]);
  //if (double(int(finalParams[6])) != finalParams[6]) {
  //  std::cerr << "Error! Number of observed events ( = " << finalParams[6] << " ) must be an integer!" << std::endl;
  //  exit(1);
  //}

  // figure out limits
  double xmin[4];
  double xmax[4];

  double width_range = 1e-30;
  //double width_range = 1e-25;
  //double width_range = 1e-20;
  double integral_range = 5;

  xmin[0] = 0;
  xmin[1] = finalParams[0] - (integral_range * finalParams[1]);
  xmin[2] = finalParams[2] - (integral_range * finalParams[3]);
  xmin[3] = finalParams[4] - (integral_range * finalParams[5]);

  xmax[0] = width_range;
  xmax[1] = finalParams[0] + (integral_range * finalParams[1]);
  xmax[2] = finalParams[2] + (integral_range * finalParams[3]);
  xmax[3] = finalParams[4] + (integral_range * finalParams[5]);

  if (xmin[1] < 0) xmin[1] = 0;
  if (xmin[2] < 0) xmin[2] = 0;
  if (xmin[3] < 0) xmin[3] = 0;

  if (xmax[2] > 1) xmax[2] = 1;

  if (verbose) print(finalParams);


  //std::cout << "Background rate is " << finalParams[4] << ". " << std::endl;
  //if (finalParams[4] < poisson_threshold) std::cout << "Using Poisson distribution!" << std::endl;
  //else std::cout << "Using Gaussian distribution!" << std::endl;

  double accuracy = 1e-3;

  double val, err;
  int keep_going;
  // int keep_going = hcubature(1,f_combined,&finalParams,4,xmin,xmax,0,0,accuracy,ERROR_INDIVIDUAL,&val,&err);
  if (finalParams[4] < poisson_threshold) keep_going = hcubature(1,f_poisson,&finalParams,4,xmin,xmax,0,0,accuracy,ERROR_INDIVIDUAL,&val,&err);
  else keep_going = hcubature(1,f_gaussian,&finalParams,4,xmin,xmax,0,0,accuracy,ERROR_INDIVIDUAL,&val,&err);

  if (val == 0) {
    width_range = 1e-35; 
    xmax[0] = width_range;
    // keep_going = hcubature(1,f_combined,&finalParams,4,xmin,xmax,0,0,accuracy,ERROR_INDIVIDUAL,&val,&err);
    if (finalParams[4] < poisson_threshold) keep_going = hcubature(1,f_poisson,&finalParams,4,xmin,xmax,0,0,accuracy,ERROR_INDIVIDUAL,&val,&err);
    else keep_going = hcubature(1,f_gaussian,&finalParams,4,xmin,xmax,0,0,accuracy,ERROR_INDIVIDUAL,&val,&err);
  }

  double norm = 1 / val;
  double minimum = 0;
  double maximum = width_range;
  //double maximum = 1e-25;
  double width;

  //std::cout << "Background rate is " << finalParams[4] << " events." << std::endl;

  while (true) {
    clock_t now = clock();
    if (double(now - start)/CLOCKS_PER_SEC > 120) return 1;
    if (keep_going != 0) break;
    xmax[0] = minimum + ((maximum - minimum) / 2);
    if (verbose) std::cout << "Running hcubature in range " << xmin[0] << " -> " << xmax[0] << "." << std::endl;
    // keep_going = hcubature(1,f_combined,&finalParams,4,xmin,xmax,0,0,accuracy,ERROR_INDIVIDUAL,&val,&err);
    if (finalParams[4] < poisson_threshold) keep_going = hcubature(1, f_poisson, &finalParams, 4, xmin, xmax, 0, 0, accuracy, ERROR_INDIVIDUAL, &val, &err);
    else keep_going = hcubature(1, f_gaussian, &finalParams, 4, xmin, xmax, 0, 0, accuracy, ERROR_INDIVIDUAL, &val, &err);
    if (verbose) std::cout << "Current val is " << val << "." << std::endl;
    double diff = (val * norm) - 0.9;
    //std::cout << "Current val is " << val << ", current diff is " << diff << "." << std::endl;
    //std::cout << "Current width is " << xmax[0] << ", current diff is " << diff << "." << std::endl;
    if (std::abs(diff) < accuracy) {
      width = xmax[0];
      keep_going = 1;
    }
    else if (diff > 0) maximum = xmax[0];
    else if (diff < 0) minimum = xmax[0];
  }

  double limit = 1 / width;
  std::cout << run_name.c_str() << " " << limit << std::endl;
}

