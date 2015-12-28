#!/bin/bash

# USAGE

usage="
Usage: $(basename "$0")

        -- Script to makeup failed jobs for nnbar limit calculation --

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
outdir="/uboone/data/users/jhewes15/limit"
export WORKDIR_HOME=${workdir}

input_file=${workdir}/input/${filename}.txt
n=`wc -l ${input_file} | cut -f1 -d " " `

for i in `seq 1 $n`; do
  params=`sed -n "${i} p" ${input_file}`

  name=`echo ${params} | cut -f1 -d " "`
  val_exp=`echo ${params} | cut -f2 -d " "`
  sig_exp=`echo ${params} | cut -f3 -d " "`
  val_eff=`echo ${params} | cut -f4 -d " "`
  sig_eff=`echo ${params} | cut -f5 -d " "`
  val_bkg=`echo ${params} | cut -f6 -d " "`
  sig_bkg=`echo ${params} | cut -f7 -d " "`

  if [ -z "${name}" ]; then
    continue
  else
    params=${val_exp},${sig_exp},${val_eff},${sig_eff},${val_bkg},${sig_bkg},${val_bkg}
    for process in {0..999}; do
      if [ ! -f ${outdir}/integral_${name}_${process}.txt ]; then
        echo "${outdir}/integral_${name}_${process}.txt does not exist!"
        echo jobsub_submit -G uboone --OS=SL6 --resource-provides=usage_model=opportunistic -e WORKDIR_HOME -dOUT ${outdir} file://${workdir}/RunSingle.sh ${name} ${params} ${process}
        jobsub_submit -G uboone --OS=SL6 --resource-provides=usage_model=opportunistic -e WORKDIR_HOME -dOUT ${outdir} file://${workdir}/RunSingle.sh ${name} ${params} ${process}
      fi
    done  
  fi
done

