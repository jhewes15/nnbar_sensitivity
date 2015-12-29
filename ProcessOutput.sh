#!/bin/bash

# USAGE

usage="
Usage: $(basename "$0")

        -- Script to process the output for nnbar limit calculation --

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
fi

workdir=`pwd`
indir="/uboone/data/users/jhewes15/limit"
outdir="${workdir}/output"

input_file=${workdir}/input/${filename}.txt
n=`wc -l ${input_file} | cut -f1 -d " " `

for i in `seq 1 $n`; do
  params=`sed -n "${i} p" ${input_file}`

  name=`echo ${params} | cut -f1 -d " "`

  outfile="${outdir}/integral_${name}.txt"
  if [ -f ${outfile} ]; then
    rm ${outfile}
  fi

  if [ -z "${name}" ]; then
    continue
  else
    for process in {0..999}; do
      infile="${indir}/integral_${name}_${process}.txt"
      cat ${infile} >> ${outfile}
      echo "" >> ${outfile}
    done
  fi
done
