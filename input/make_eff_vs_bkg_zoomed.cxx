

void make_eff_vs_bkg_zoomed() {

  ofstream f;

  f.open("eff_vs_bkg_zoomed.txt");

  double n_eff = 100;
  double n_bkg = 100;

  double eff_min = 0.5;
  double eff_max = 1;

  double bkg_min = 0;
  double bkg_max = 0.0625;

  double eff_u = (eff_max - eff_min) / n_eff;
  double bkg_u = (bkg_max - bkg_min) / n_bkg;

  for (int i = 0; i < n_eff; i++) {
    double eff = eff_min + ((i + 1) * eff_u);
    for (int j = 0; j < n_bkg; j++) {
      double bkg = bkg_min + ((j + 1) * bkg_u);
      f << i << "_" << j << " 4 0.03 " << eff << " 0.25 " << bkg << " 0.25\n";
    }
  }
  f.close();
}
