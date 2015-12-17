#!/bin/bash

usage="
Usage: $(basename "$0")

        -- Script to perform nnbar limit calculations from integral output --

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

if [ -z "${filename}" ]; then
  echo "
You must specify a run name or list of run names using -r"
  echo "${usage}"
  exit 1
fi

workdir=`pwd`

if [ -e ${workdir}/input/${filename}.txt ]; then
  rm ./limit_vs_exposure.txt
  filename=${workdir}/input/${filename}.txt
  echo "Running for list of inputs ${filename}"
  a=`wc -l ${filename}`
  b=`echo ${a} | cut -f1 -d " "`
  for i in `seq 1 ${b}`; do
    run_name=`sed -n "${i} p" ${filename} | cut -f1 -d " "`
    if [ -z "${run_name}" ]; then
      continue
    else
      echo "
Running for run name ${run_name}"
      limit=`root -l -b -q ${workdir}/src/CalculateLimit.cxx\(\"${run_name}\",\"${workdir}\"\) | cat | cut -f5 -d " " | cut -f1 -d "!" | tr -d '\n'`
      
      exposure=`sed -n "${i} p" ${filename} | cut -f2 -d " " | tr -d '\n'`
      echo "${exposure} ${limit}" >> ${workdir}/plots/limit_vs_exposure.txt
    fi
  done
elif [ -e ${workdir}/output/integral_${filename}_0.txt ]; then
  echo "Running for input ${filename}"
  root -l -b -q ${workdir}/src/CalculateLimit.cxx\(\"${filename}\",\"${workdir}\"\) > ./plots/${run_name}.txt
else
  echo "Couldn't find any input or list of inputs associated with ${filename}"
fi

