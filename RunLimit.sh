#!/bin/bash

run_name=${1}
params=${2}

umask +w
source /grid/fermiapp/products/uboone/setup_uboone.sh
setup root
setup ifdhc v1_7_2

WORKDIR_GRID=${CONDOR_DIR_INPUT}

val_exp=`echo ${params} | cut -f1 -d ","`
sig_exp=`echo ${params} | cut -f2 -d ","`
val_eff=`echo ${params} | cut -f3 -d ","`
sig_eff=`echo ${params} | cut -f5 -d ","`
val_bkg=`echo ${params} | cut -f5 -d ","`
sig_bkg=`echo ${params} | cut -f6 -d ","`

echo "RunLimit.sh parameter summary:"
echo "  Exp     - ${val_exp}"
echo "  Exp_sig - ${sig_exp}"
echo "  Eff     - ${val_eff}"
echo "  Eff_sig - ${sig_eff}"
echo "  Bkg     - ${val_bkg}"
echo "  Bkg_sig - ${sig_bkg}"

if [ ! -d ${output} ]; then
  mkdir output
fi

ifdh cp -D ${WORKDIR_HOME}/src/* ${WORKDIR_GRID}

echo root -b -q ${WORKDIR_GRID}/Limit.cxx\(\"${run_name}\",${val_exp},${sig_exp},${val_eff},${sig_eff},${val_bkg},${sig_bkg},${PROCESS}\)
root -b -q ${WORKDIR_GRID}/Limit.cxx\(\"${run_name}\",${val_exp},${sig_exp},${val_eff},${sig_eff},${val_bkg},${sig_bkg},${PROCESS}\)

