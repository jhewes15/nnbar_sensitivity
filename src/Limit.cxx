#include "Integrator.cxx"
#include <string>
#include <fstream>

// code to run the limit calculation
void Limit(string run_name, double valExp, double sigExp, double valEff, double sigEff, double valBkg, double sigBkg, int process)
{
  std::vector<double> params;
  params.reserve(6);
  params.push_back(valExp);
  params.push_back(sigExp);
  params.push_back(valEff);
  params.push_back(sigEff);
  params.push_back(valBkg);
  params.push_back(sigBkg);

  // loop over, do it 10 times
  for (int i = 0; i < 10; i++)
  {
    int study = (process * 10) + i;
    
    Integrator * ig = new Integrator();

    ig->SetProcess(study);
    ig->SetParams(params);
    ig->PrintParams();
    ig->FindWidthRange();

    std::vector<double> lifetime = ig->Limit();

    ofstream outfile;
    TString outfilename = Form("%s/integral_%s_%i.txt", getenv("CONDOR_DIR_OUT"), run_name, study);
    std::cout << "Outfilename is " << outfilename.Data() << std::endl;
    outfile.open(outfilename.Data());
    outfile << integral[0] << " " << integral[1];
    outfile.close();

    delete ig;
  }
}

