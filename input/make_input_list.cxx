

void make_input_list() {

  ofstream f;

  f.open("eff_vs_bkg.txt");

  double n_eff = 100;
  double n_bkg = 100;

  double eff_total = 1;
  double bkg_total = 0.25;

  double eff_u = eff_total / n_eff;
  double bkg_u = bkg_total / n_bkg;

  for (int i = 0; i < n_eff; i++) {
    double eff = (i + 1) * eff_u;
    for (int j = 0; j < n_bkg; j++) {
      double bkg = (j + 1) * bkg_u;
      f << i << "_" << j << " 4 0.03 " << eff << " 0.25 " << bkg << " 0.25\n";
    }
  }
  f.close();
}
