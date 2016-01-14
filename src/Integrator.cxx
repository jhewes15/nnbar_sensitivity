#ifndef INTEGRATOR_CXX
#define INTEGRATOR_CXX

#include "Integrator.h"

// Standard constructor
Integrator::Integrator()
{
  _verbose = false;
//  _mass    = 20;           // kt
//  _density = 3.3164892e32; // neutrons / kt

// adding in super-k parameters for debugging purposes
  _mass    = 22.5;          // kt
  _density = 2.6742369e+32; // neutrons / kt
}

// Set input parameters
void Integrator::SetParams(std::vector<double> params)
{
  if (params.size() != 6)
  {
    std::cerr << "Input parameter vector is the wrong size! Should contain 6 values." << std::endl;
    exit(1);
  }
  
  _inputParams = params;
  
  // Convert input values into their final form
  _finalParams.clear();
  _finalParams.reserve(7);
  
  // Exposure value & uncertainty
  _finalParams.push_back(_inputParams[0] * _mass * _density);
  _finalParams.push_back(_inputParams[1] * _finalParams[0]);
  
  // Efficiency value & uncertainty
  _finalParams.push_back(_inputParams[2]);
  _finalParams.push_back(_inputParams[3] * _finalParams[2]);
  
  // Background value & uncertainty
  _finalParams.push_back(_inputParams[4] * _mass * _inputParams[0]);
  _finalParams.push_back(_inputParams[5] * _finalParams[4]);
  
  // Number of events
  _finalParams.push_back(_finalParams[5]);
  
  // Calculate integration limits
  double min, max;
  _intervals.clear();
  _intervals.reserve(6);
  
  for (int i = 0; i < 3; i++)
  {
    min = _finalParams[2*i] - (5 * _finalParams[(2*i)+1]);
    if (min < 0) min = 0;
    max = _finalParams[2*i] + (5 * _finalParams[(2*i)+1]);
    _intervals.push_back(min);
    _intervals.push_back(0.01 * (max - min));
  }
}

void Integrator::CheckParams()
{
  if (_inputParams.size() == 6 && _finalParams.size() == 7) return;
  else
  {
    std::cerr << "Please set parameters using SetParams before calling this function." << std::endl;
    exit(1);
  }
}

void Integrator::FindWidthRange()
{
  CheckParams();
  
  globalParams = _finalParams;
  
  // Set up 3D integrator
  ROOT::Math::Functor f3D(Integral3D, 3);
  ROOT::Math::IntegratorMultiDim ig3D(ROOT::Math::IntegrationMultiDim::kADAPTIVE);
  ROOT::Math::IntegratorMultiDim ig3D(f3D);
  ig3D.SetRelTolerance(0.01);
  
  // Set lower limits
  double xmin3D[3];
  double xmax3D[3];

  xmin3D[0] = _intervals[0];
  xmax3D[0] = xmin3D[0] + (100 * _intervals[1]);
  xmin3D[1] = _intervals[2];
  xmax3D[1] = xmin3D[1] + (100 * _intervals[3]);
  xmin3D[2] = _intervals[4];
  xmax3D[2] = xmin3D[2] + (100 * _intervals[5]);


  globalWidthMax = 1e-40;
  double cut_value = 1e-5 * ig3D.Integral(xmin3D, xmax3D);

  for (int i = 0; i < 10; i++) {
    globalWidthMax = pow(10, -39 + i);
    if (ig3D.Integral(xmin3D, xmax3D) < cut_value) {
      _widthMax = globalWidthMax;
      return;
    }
  }
  std::cerr << "If you made it to the end of this function, it means something went wrong. Exiting..." << std::endl;
  exit(1);
}

// Limit calculation function
std::vector<double> Integrator::Limit()
{
  CheckParams();
  
  // Set up 4D integrator
  ROOT::Math::Functor f4D(Integral4D, 4);
  ROOT::Math::IntegratorMultiDim ig4D(ROOT::Math::IntegrationMultiDim::kADAPTIVE);
  ROOT::Math::IntegratorMultiDim ig4D(f4D);
  ig4D.SetRelTolerance(0.01);
  
  // Set limits for 4D integration
  double xmin4D[4];
  xmin4D[0] = _process * 0.001 * _widthMax;
  
  double xmax4D[4];
  xmax4D[0] = xmin4D[0] + (0.001 * _widthMax);
  
  double integral = 0;
  double width = (_process + 0.5) * 0.001 * _widthMax;
  
  // Split three remaining integration dimensions into 100 small integrals
  for (int i_exp = 0; i_exp < 100; i_exp++)
  {
    xmin4D[1] = _intervals[0] + (i_exp * _intervals[1]);
    xmax4D[1] = xmin4D[1] + _intervals[1];
    for (int i_eff = 0; i_eff < 100; i_eff++)
    {
      xmin4D[2] = _intervals[2] + (i_eff * _intervals[3]);
      xmax4D[2] = xmin4D[2] + _intervals[3];
      for (int i_bkg = 0; i_bkg < 100; i_bkg++)
      {
        xmin4D[3] = _intervals[4] + (i_bkg * _intervals[5]);
        xmax4D[3] = xmin4D[3] + _intervals[5];
        integral += ig4D.Integral(xmin4D, xmax4D);
      }
    }
  }
  
  std::vector<double> out;
  out.push_back(width);
  out.push_back(integral);
  return out;
}

// info-dump function
void Integrator::PrintParams()
{
  std::cout << std::endl;
  CheckParams();
  
  std::cout << "  Exposure: "   << _inputParams[0]       << " kt.yrs ± "      << 100 * _inputParams[1] << " %" << std::endl;
  std::cout << "  Efficiency: " << 100 * _inputParams[2] << " %  ± "          << 100 * _inputParams[3] << " %" << std::endl;
  std::cout << "  Background: " << _inputParams[4]       << " (kt.yrs)^-1 ± " << 100 * _inputParams[5] << " %" << std::endl;
  
  std::cout << std::endl;
  
  std::cout << "  Mass: "    << _mass    << " kt"                 << std::endl;
  std::cout << "  Density: " << _density << " neutrons / (kt.yr)" << std::endl;
  
  std::cout << std::endl;
  
  std::cout << "  Total exposure: "   << _finalParams[0]       << " ± " << _finalParams[1]       << " neutrons" << std::endl;
  std::cout << "  Total efficiency: " << 100 * _finalParams[2] << " ± " << 100 * _finalParams[3] << " %"        << std::endl;
  std::cout << "  Total background: " << _finalParams[4]       << " ± " << _finalParams[5]       << " events"   << std::endl;
  std::cout << "  Event rate: "       << _finalParams[6]                                         << " events"   << std::endl; 
  std::cout << std::endl;
}

#endif
