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
        -n            Select a subset to process (optional)
        -t            Test mode - output to terminal instead of file
"

# Parse optional arguments.

while getopts "hr:n:t" option; do
  case "${option}" in
    h)  echo "${usage}"
        exit
        ;;
    r)  filename=${OPTARG}
        ;;
    n)  subset=${OPTARG}
        ;;
    t)  test="Y"
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

workdir=/Users/jhewes15/neutrino/nnbar_sensitivity

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

if [ ! -z "${subset}" ]; then
  subset=$((subset - 1))
  min=$((1000 * subset))
  max=$((min + 1000))
  min=$((min + 1))
else
  min=1
  max=${n}
fi

for i in `seq $min $max`; do
  params=`sed -n "${i} p" ${infile}`

  name=`echo ${params}    | cut -f1 -d " "`
  val_exp=`echo ${params} | cut -f2 -d " "`
  sig_exp=`echo ${params} | cut -f3 -d " "`
  val_eff=`echo ${params} | cut -f4 -d " "`
  sig_eff=`echo ${params} | cut -f5 -d " "`
  val_bkg=`echo ${params} | cut -f6 -d " "`
  sig_bkg=`echo ${params} | cut -f7 -d " "`

  if [ ! -z "${name}" ]; then
    if [ -z "${test}" ]; then
      ${workdir}/src/limit ${name} ${val_exp} ${sig_exp} ${val_eff} ${sig_eff} ${val_bkg} ${sig_bkg} >> ${outfile}
    else
      ${workdir}/src/limit ${name} ${val_exp} ${sig_exp} ${val_eff} ${sig_eff} ${val_bkg} ${sig_bkg}
    fi
    time=`date +"%T"`
    echo "Done calculating limit for run ${name}; timestamp is ${time}"
  fi

done

