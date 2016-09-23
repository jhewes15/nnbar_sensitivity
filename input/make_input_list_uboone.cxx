

void make_input_list_uboone() {

  ofstream f;

  f.open("eff_vs_bkg_uboone.txt");

  double n_eff = 100;
  double n_bkg = 41;

  double eff_total = 1;
  double bkg_total = 0.25;

  double eff_u = eff_total / n_eff;

  for (int i = 0; i < n_eff; i++) {
    double eff = (i + 1) * eff_u;
    for (int j = 0; j < n_bkg; j++) {
      double bkg = pow(10,float(j)/10.)/0.085;
      f << i << "_" << j << " 6.66e-6 0.03 " << eff << " 0.25 " << bkg << " 0.25\n";
    }
  }
  f.close();
}
