#!/bin/bash

PROCESS=0

run_name=${1}
params=${2}

WORKDIR=`pwd`

if [ -z "${params}" ]; then
  echo Using Super-K parameters
else
  echo Using custom parameters
  val_exp=`echo ${params} | cut -f1 -d ","`
  sig_exp=`echo ${params} | cut -f2 -d ","`
  val_eff=`echo ${params} | cut -f3 -d ","`
  sig_eff=`echo ${params} | cut -f4 -d ","`
  val_bkg=`echo ${params} | cut -f5 -d ","`
  sig_bkg=`echo ${params} | cut -f6 -d ","`
fi

echo "LimitDev.sh parameter summary:"
echo "  Exp     - ${val_exp}"
echo "  Exp_sig - ${sig_exp}"
echo "  Eff     - ${val_eff}"
echo "  Eff_sig - ${sig_eff}"
echo "  Bkg     - ${val_bkg}"
echo "  Bkg_sig - ${sig_bkg}"

echo root -b -q ${WORKDIR}/src/RunStudy.cxx\(${PROCESS},\"${run_name}\",${val_exp},${sig_exp},${val_eff},${sig_eff},${val_bkg},${sig_bkg}\)
root -b -q ${WORKDIR}/src/RunStudy.cxx\(${PROCESS},\"${run_name}\",${val_exp},${sig_exp},${val_eff},${sig_eff},${val_bkg},${sig_bkg}\)

