#include <fstream>

void EffVsBkgZoomed() {
  
  gStyle->SetOptStat(0);
  
  string input_file = "../output/eff_vs_bkg_zoomed.txt";
    
  ifstream infile;
  infile.open(input_file.c_str());
  if (!infile) {
    std::cerr << "Could not open input file " << input_file.c_str() << " for reading." << std::endl;
    return;
  }
  
  TH2 * h = new TH2D("h", "", 100, 50, 100, 100, 0, 10);
  
  string line;

  while(std::getline(infile, line, '\n')) {
    string name;
    double lifetime;

    if (line != "") {
      name = line.substr(0, line.find(" "));
      lifetime = stod(line.substr(line.find(" ") + 1, line.size()));
      int i = stoi(name.substr(0, name.find("_"))) + 1;
      int j = stoi(name.substr(name.find("_") + 1, name.size())) + 1;
      h->SetBinContent(i, j, lifetime);
    }
  }
  
  TCanvas * c = new TCanvas ("c", "", 1600, 800);

  // gonna try smoothing
  //h->Smooth();

  h->Draw("colz");

  // gonna try a contour plot
//  h->Draw("cont4z");
  gStyle->SetNumberContours(99);

  // also gonna try a log scale
//  c->SetLogz();

  // draw a super-k point on the plot
  // taken out, just gonna copy & paste it :/
/*  TMarker * m = new TMarker(12, 24.1, 29);
  m->SetMarkerSize(10);
  m->SetMarkerColor(kMagenta);
  m->Draw();
*/
//  c->Update();

  h->GetXaxis()->SetTitle("Efficiency (%)");
  h->GetXaxis()->SetTitleSize(0.05);
  h->GetYaxis()->SetTitle("Background rate");
  h->GetYaxis()->SetTitleSize(0.05);
  h->GetZaxis()->SetTitle("90% CL lifetime limit");
  //h->SetTitle("nnbar sensitivity, efficiency vs background");
  c->SaveAs("eff_vs_bkg_zoomed.png");
}
