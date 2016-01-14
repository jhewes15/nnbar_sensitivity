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
  n_obs=`echo ${params}   | cut -f8 -d " "`

  if [ -z "${name}" ]; then
    continue
  else
    echo "SubmitMultiple.sh parameter summary:"
    echo "  Exp     - ${val_exp}"
    echo "  Exp_sig - ${sig_exp}"
    echo "  Eff     - ${val_eff}"
    echo "  Eff_sig - ${sig_eff}"
    echo "  Bkg     - ${val_bkg}"
    echo "  Bkg_sig - ${sig_bkg}"

    if [ -z "${n_obs}" ]; then
      params=${val_exp},${sig_exp},${val_eff},${sig_eff},${val_bkg},${sig_bkg},${val_bkg}
    else
      echo "  N_obs   - ${n_obs}"
      params=${val_exp},${sig_exp},${val_eff},${sig_eff},${val_bkg},${sig_bkg},${val_bkg},${n_obs}
    fi

    echo jobsub_submit -G uboone --OS=SL6 --resource-provides=usage_model=opportunistic -N 100 -e WORKDIR_HOME -dOUT ${outdir} file://${workdir}/RunLimit.sh ${name} ${params}
    jobsub_submit -G uboone --OS=SL6 --resource-provides=usage_model=opportunistic -N 100 -e WORKDIR_HOME -dOUT ${outdir} file://${workdir}/RunLimit.sh ${name} ${params}
  fi
done

