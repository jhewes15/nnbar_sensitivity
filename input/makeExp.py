
f = open('./exposure.txt', 'w')

for exp_val in range(1,11):

  exp_sig = 0.03

  eff_val = 1
  eff_sig = 0.25

  bkg_val = 0.25
  bkg_sig = 0.25

  f.write('{} {} {} {} {} {} {}\n'.format(exp_val, exp_val, exp_sig, eff_val, eff_sig, bkg_val, bkg_sig))

