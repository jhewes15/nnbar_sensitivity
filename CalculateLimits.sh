#!/bin/bash

usage="
Usage: $(basename "$0")

        -- Script perform an nnbar limit calculation on output --

    OPTIONS:

        -h            Show this help page.
        -r            Input run name / input filename
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

workdir=`pwd`

if [ -e ${workdir}/${filename}.txt ]; then
  filename=${workdir}/${filename}.txt
  echo "Running for list of inputs ${filename}"
  n=`wc -l ${filename}`
  n=`echo ${n} | cut -f1 -d " "`
  for i in `seq 1 ${n}`; do
    run_name=`sed -n "${i} p" ${filename} | cut -f1 -d " "`
    echo
    echo
    echo "Running for run name ${run_name}"
    root -b -q CalculateLimit.cxx\(\"${run_name}\",\"${workdir}\"\)
  done
elif [ -e ${workdir}/output/integral_${filename}_0.txt ]; then
  echo "Running for input ${filename}"
  root -l -b -q CalculateLimit.cxx\(\"${filename}\",\"${workdir}\"\)
else
  echo "Couldn't find any input or list of inputs associated with ${filename}"
fi

