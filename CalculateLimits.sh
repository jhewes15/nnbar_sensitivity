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
output="${workdir}/plots/${filename}.txt"

if [ -f ${output} ]; then
  rm ${output}
fi

if [ -e ${workdir}/input/${filename}.txt ]; then
  filename=${workdir}/input/${filename}.txt
  echo "Running for list of inputs ${filename}"
  a=`wc -l ${filename}`
  b=`echo ${a} | cut -f1 -d " "`
  for i in `seq 1 ${b}`; do
    run_name=`sed -n "${i} p" ${filename} | cut -f1 -d " "`
    if [ -z "${run_name}" ]; then
      continue
    else
      limit=`root -l -b -q ${workdir}/src/CalculateLimit.cxx\(\"${run_name}\",\"${workdir}\"\) | cat | sed -n 3p`
      echo "${run_name} ${limit}" >> ${output}
    fi
  done
elif [ -e ${workdir}/output/integral_${filename}_0.txt ]; then
  echo "Running for input ${filename}"
  root -l -b -q ${workdir}/src/CalculateLimit.cxx\(\"${filename}\",\"${workdir}\"\)
else
  echo "Couldn't find any input or list of inputs associated with ${filename}"
fi

