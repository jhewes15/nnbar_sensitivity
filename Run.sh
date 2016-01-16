#!/bin/bash

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

shift $((OPTIND - 1))

if [ -z "${filename}" ]; then
  echo "
You must specify a filename using the -r option!"
  echo "${usage}"
  exit 1
fi

workdir=`pwd`

infile=${workdir}/input/${filename}.txt
outfile=${workdir}/output/${filename}.txt
if [ -f ${outfile} ]; then
  rm ${outfile}
fi
if [ ! -f ${workdir}/src/limit ]; then
  echo "Error! You must build src/limit before running this script!"
  exit 1
fi

n=`wc -l ${infile} | sed -e 's/^[ \t]*//' | cut -f1 -d " "`

for i in `seq 1 $n`; do
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
    ${workdir}/src/limit ${name} ${val_exp} ${sig_exp} ${val_eff} ${sig_eff} ${val_bkg} ${sig_bkg} ${n_obs} >> ${outfile}
    time=`date +"%T"`
    echo "Done calculating limit for run ${name}; timestamp is ${time}"
  fi

done

