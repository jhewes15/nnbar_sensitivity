#include <fstream>

void CalculateLimit(TString run_name, TString workdir)
{
  // Define some variables
  double lifetime[1000];
  double integral[1000];
  double cumulative_integral[1000];
  double total = 0;
  TString output_dir = Form("%s/output", workdir.Data());

  // Loop over input files
  for (int i = 0; i < 1000; i++) {

    // Open input file and read out info
    TString filename = Form("%s/integral_%s_%i.txt", output_dir.Data(), run_name.Data(), i);
    ifstream infile;
    infile.open(filename.Data());
    if (!infile) {
      std::cout << "Could not open input file " << filename.Data() << " for reading." << std::endl;
      return;
    }
    infile >> lifetime[i] >> integral[i];
    infile.close();

    // Add up cumulative integral
    total += integral[i];
    cumulative_integral[i] = total;
  }

  // Get integration constant
  double A = cumulative_integral[999];
  double max = 0.9 * A;

  if (A == 0) {
    std::cout << "The total integral is zero, so no limit was found." << std::endl;
    return;
  }

  bool found_limit = false;

  // Find 90% confidence level!
  for (int i = 0; i < 1000; i++) { 
    if (cumulative_integral[i] > max) {
      found_limit = true;
      double limit = 1 / lifetime[i];
      std::cout << "Calculated lifetime limit is " << limit << "!" << std::endl;
      break;
    }
  }

  if (found_limit) {
    TCanvas * c = new TCanvas("c", "", 2000, 1600);
    TGraph * g_cumulative_4D = new TGraph(1000, lifetime, cumulative_integral);
    g_cumulative_4D->SetLineWidth(3);
    g_cumulative_4D->Draw("ac");
    g_cumulative_4D->SetTitle("Cumulative Bayesian probability distribution");
    g_cumulative_4D->GetXaxis()->SetTitle("Upper limit of integration (yrs^{-1})");
    g_cumulative_4D->GetYaxis()->SetTitle("Integral value");
    c->SaveAs(Form("./plots/%s.png", run_name.Data()));
  }
}
