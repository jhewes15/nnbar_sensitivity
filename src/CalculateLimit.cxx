#include <fstream>
#include <string>

double CalculateLimit(string run_name, string workdir)
{
  // Define some variables
  double width[1000];
  double integral[1000];
  double cumulative_integral[1000];
  double total = 0;
  string output_dir = workdir + "/output";

  // Open input file and read out info
  string filename = output_dir + "/integral_" + run_name + ".txt";
  ifstream infile;
  infile.open(filename.c_str());
  if (!infile) {
    std::cout << "Could not open input file " << filename.c_str() << " for reading." << std::endl;
    return;
  }

  // Loop over input files
  for (int i = 0; i < 1000; i++) {
    infile >> width[i] >> integral[i];

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
      double lifetime = 1 / width[i];
      std::cout << lifetime << std::endl;
      break;
    }
  }

  if (found_limit) {
    TCanvas * c = new TCanvas("c", "", 2000, 1600);
    TGraph * g_cumulative_4D = new TGraph(1000, width, cumulative_integral);
    g_cumulative_4D->SetLineWidth(3);
    g_cumulative_4D->Draw("ac");
    g_cumulative_4D->SetTitle("Cumulative Bayesian probability distribution");
    g_cumulative_4D->GetXaxis()->SetTitle("Upper limit of integration (yrs^{-1})");
    g_cumulative_4D->GetYaxis()->SetTitle("Integral value");
    c->SaveAs(Form("./plots/cumulative_%s.png", run_name.c_str()));
  }
}
