#!/bin/bash

# USAGE

usage="
Usage: $(basename "$0")

        -- Script to run a single slice of an nnbar limit calculation --

    OPTIONS:

        -h            Show this help page.
        -r            Run name
        -p            Parameters
        -n            Process number
"

# Parse optional arguments.

while getopts "hr:p:n:" option; do
  case "${option}" in
    h)  echo "${usage}"
        exit
        ;;
    r)  run_name=${OPTARG}
        ;;
    p)  params=${OPTARG}
        ;;
    n)  process=${OPTARG}
        ;;
    :)  printf "Missing argument for -%s\n" "${OPTARG}" >&2
        echo "${usage}" >&2
        exit 1
        ;;
    \?) printf "Illegal option: -%s\n" "${OPTARG}" >&2
        echo "${usage}" >&2
        exit 1
        ;;
  esac
done

shift $((OPTIND - 1))

if [ -z "${run_name}" ]; then
  echo "
You must specify a filename using the -r option!"
  echo "${usage}"
  exit 1
else if [ -z "${params}" ]; then
  echo "
You must provide parameters using the -p option!"
  echo "${usage}"
  exit 1
else if [ -z "${process}" ]; then
  echo "
You must provide process number using the -n option!"
  echo "${usage}"
  exit 1
fi

val_exp=`echo ${params} | cut -f1 -d ","`
sig_exp=`echo ${params} | cut -f2 -d ","`
val_eff=`echo ${params} | cut -f3 -d ","`
sig_eff=`echo ${params} | cut -f5 -d ","`
val_bkg=`echo ${params} | cut -f5 -d ","`
sig_bkg=`echo ${params} | cut -f6 -d ","`

workdir=`pwd`
outdir="/uboone/data/users/jhewes15/limit"

echo root -b -q ${workdir}/src/Single.cxx\(\"${run_name}\",${val_exp},${sig_exp},${val_eff},${sig_eff},${val_bkg},${sig_bkg},${process}\)
root -b -q ${workdir}/src/Single.cxx\(\"${run_name}\",${val_exp},${sig_exp},${val_eff},${sig_eff},${val_bkg},${sig_bkg},${process}\)

