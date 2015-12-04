#!/bin/bash

run_name=${1}
params=${2}

umask +w
source /grid/fermiapp/products/uboone/setup_uboone.sh
setup root

WORKDIR_GRID=${CONDOR_DIR_INPUT}

if [ -z "${params}" ]; then
  echo Using Super-K parameters
else
  echo Using custom parameters
  eff=`echo ${params} | cut -f1 -d ","`
  sigma_eff=`echo ${params} | cut -f2 -d ","`
  exp=`echo ${params} | cut -f3 -d ","`
  sigma_exp=`echo ${params} | cut -f4 -d ","`
  bkg=`echo ${params} | cut -f5 -d ","`
  sigma_bkg=`echo ${params} | cut -f6 -d ","`
  n_obs=`echo ${params} | cut -f7 -d ","`
fi

mkdir output

if [ -z "${params}" ]; then
  echo root -b -q ${WORKDIR_HOME}/Limit.cxx\(${PROCESS},\"${run_name}\"\)
  root -b -q ${WORKDIR_HOME}/Limit.cxx\(${PROCESS},\"${run_name}\"\)
else
  echo root -b -q ${WORKDIR_HOME}/Limit.cxx\(${PROCESS},\"${run_name}\",true,${eff},${sigma_eff},${exp},${sigma_exp},${bkg},${sigma_bkg},${n_obs}\)
  root -b -q ${WORKDIR_HOME}/Limit.cxx\(${PROCESS},\"${run_name}\",true,${eff},${sigma_eff},${exp},${sigma_exp},${bkg},${sigma_bkg},${n_obs}\)
fi

