#include "Integrator.cxx"
#include <string>
#include <fstream>

// code to run a single piece of the limit calculation
void Single(string run_name, double valExp, double sigExp, double valEff, double sigEff, double valBkg, double sigBkg, int process)
{
  std::vector<double> params;
  params.reserve(6);
  params.push_back(valExp);
  params.push_back(sigExp);
  params.push_back(valEff);
  params.push_back(sigEff);
  params.push_back(valBkg);
  params.push_back(sigBkg);

  Integrator * ig = new Integrator();

  ig->SetProcess(process);
  ig->SetParams(params);
  ig->PrintParams();
  ig->FindWidthRange();

  std::vector<double> integral = ig->Limit();

  ofstream outfile;
  TString outfilename = Form("%s/integral_%s_%i.txt", getenv("CONDOR_DIR_OUT"), run_name.c_str(), process);
  std::cout << "Outfilename is " << outfilename.Data() << std::endl;
  outfile.open(outfilename.Data());
  outfile << integral[0] << " " << integral[1];
  outfile.close();

  delete ig;
}

