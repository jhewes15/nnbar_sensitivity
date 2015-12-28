#!/bin/bash

run_name=${1}
params=${2}
process=${3}

umask +w
source /grid/fermiapp/products/uboone/setup_uboone.sh
setup root
setup ifdhc v1_7_2

WORKDIR_GRID=${CONDOR_DIR_INPUT}

val_exp=`echo ${params} | cut -f1 -d ","`
sig_exp=`echo ${params} | cut -f2 -d ","`
val_eff=`echo ${params} | cut -f3 -d ","`
sig_eff=`echo ${params} | cut -f4 -d ","`
val_bkg=`echo ${params} | cut -f5 -d ","`
sig_bkg=`echo ${params} | cut -f6 -d ","`

if [ ! -d ${output} ]; then
  mkdir output
fi

echo "ifdh cp -D ${WORKDIR_HOME}/src/* ${WORKDIR_GRID}"
ifdh cp -D ${WORKDIR_HOME}/src/* ${WORKDIR_GRID}

echo root -b -q ${WORKDIR_GRID}/Single.cxx\(\"${run_name}\",${val_exp},${sig_exp},${val_eff},${sig_eff},${val_bkg},${sig_bkg},${process}\)
root -b -q ${WORKDIR_GRID}/Single.cxx\(\"${run_name}\",${val_exp},${sig_exp},${val_eff},${sig_eff},${val_bkg},${sig_bkg},${process}\)

