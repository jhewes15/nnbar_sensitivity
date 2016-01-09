#include <fstream>

void EffVsBkg() {
  
  gStyle->SetOptStat(0);
  
  string input_file = "eff_vs_bkg.txt";
  
  std::vector<double> lifetime;
  lifetime.reserve(100);
  
  ifstream infile;
  infile.open(input_file.c_str());
  if (!infile) {
    std::cerr << "Could not open input file " << input_file.c_str << " for reading." << std::endl;
    return;
  }
  
  string temp_name;
  double temp_lifetime;
  
  TH2 * h = new TH2D("h", "", 10, 0, 1, 10, 0, 40);
  
  for (int i = 1; i < 11; i++) {
    for (int j = 1; j < 11; j++) {
      infile >> temp_name >> temp_lifetime;
      h->SetBinContent(i, j, temp_lifetime);
    }
  }
  
  TCanvas * c = new TCanvas ("c", "", 1600, 800);

  // gonna try smoothing
  h->Smooth();

  //h->Draw("colz");

  // gonna try a contour plot
  h->Draw("cont4z");
  gStyle->SetNumberContours(99);

  // also gonna try a log scale
  c->SetLogz();

  h->GetXaxis()->SetTitle("Efficiency");
  h->GetYaxis()->SetTitle("Background rate");
  h->SetTitle("nnbar sensitivity, efficiency vs background");
  c->SaveAs("eff_vs_bkg.png");
}

