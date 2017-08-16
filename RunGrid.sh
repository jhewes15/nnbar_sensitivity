#!/bin/bash

umask +w
source /grid/fermiapp/products/uboone/setup_uboone.sh
setup root
setup ifdhc v1_7_2

# USAGE

usage="
Usage: $(basename "$0")

        -- Script to run nnbar limit calculations --

    OPTIONS:

        -h            Show this help page.
        -r            Input filename (required)
                        Should be in format <current_directory>/input/<filename>.txt
                        Look at input_list.txt for an example of how this file looks
"

# Parse optional arguments.

while getopts "hr:" option; do
  case "${option}" in
    h)  echo "${usage}"
        exit
        ;;
    r)  filename=${OPTARG}
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

env

shift $((OPTIND - 1))

if [ -z "${filename}" ]; then
  echo "
You must specify a filename using the -r option!"
  echo "${usage}"
  exit 1
fi

workdir="/uboone/app/users/jhewes15/nnbar/sensitivity"

chmod 777 ${CONDOR_DIR_INPUT}/limit

infile=${CONDOR_DIR_INPUT}/${filename}.txt
echo "infile is ${infile}"

n=`wc -l ${infile} | sed -e 's/^[ \t]*//' | cut -f1 -d " "`

echo "subset is ${PROCESS}"
#min=$((40 * PROCESS))
min=$((51 * PROCESS))
#max=$((min + 40))
max=$((min + 51))
min=$((min + 1))

outfile=${CONDOR_DIR_INPUT}/${filename}_${PROCESS}.txt
echo "outfile is ${outfile}"

for i in `seq $min $max`; do
  params=`sed -n "${i} p" ${infile}`

  name=`echo ${params}    | cut -f1 -d " "`
  val_exp=`echo ${params} | cut -f2 -d " "`
  sig_exp=`echo ${params} | cut -f3 -d " "`
  val_eff=`echo ${params} | cut -f4 -d " "`
  sig_eff=`echo ${params} | cut -f5 -d " "`
  val_bkg=`echo ${params} | cut -f6 -d " "`
  sig_bkg=`echo ${params} | cut -f7 -d " "`
  n_obs=`echo ${params}   | cut -f8 -d " "`

  if [ ! -z "${name}" ]; then
    ${CONDOR_DIR_INPUT}/limit ${name} ${val_exp} ${sig_exp} ${val_eff} ${sig_eff} ${val_bkg} ${sig_bkg} ${n_obs} >> ${outfile}
  fi

done

ifdh cp -D ${outfile} /uboone/data/users/jhewes15/limit

