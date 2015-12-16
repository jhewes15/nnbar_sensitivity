#!/bin/bash

# USAGE

usage="
Usage: $(basename "$0")

        -- Script to submit jobs which perform an nnbar limit calculation --

    OPTIONS:

        -h            Show this help page.
        -r            Input filename (required)
                        Should be in format <current_directory>/<filename>.txt
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
else
  echo filename is ${filename}
fi

workdir=`pwd`

input_file=${workdir}/input/${filename}.txt
n=`wc -l ${input_file} | cut -f1 -d " " `

for i in `seq 1 $n`; do
  params=`sed -n "${i} p" ${input_file}`

  name=`echo ${params} | cut -f1 -d " "`
  val_eff=`echo ${params} | cut -f2 -d " "`
  sig_eff=`echo ${params} | cut -f3 -d " "`
  val_exp=`echo ${params} | cut -f4 -d " "`
  sig_exp=`echo ${params} | cut -f5 -d " "`
  val_bkg=`echo ${params} | cut -f6 -d " "`
  sig_bkg=`echo ${params} | cut -f7 -d " "`

  if [ -z "${name}" ]; then
    continue
  else
    echo bash ${workdir}/SubmitLimit.sh -r ${name} -p ${val_eff},${sig_eff},${val_exp},${sig_exp},${val_bkg},${sig_bkg},${val_bkg}
    bash ${workdir}/SubmitLimit.sh -r ${name} -p ${val_eff},${sig_eff},${val_exp},${sig_exp},${val_bkg},${sig_bkg},${val_bkg}
  fi
done
