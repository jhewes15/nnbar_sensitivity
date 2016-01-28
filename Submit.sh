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
        -n            Which subset to use (optional)
"

# Parse optional arguments.

while getopts "hr:n:" option; do
  case "${option}" in
    h)  echo "${usage}"
        exit
        ;;
    r)  filename=${OPTARG}
        ;;
    n)  subset=${OPTARG}
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

num_jobs=10

mail=""

echo
echo jobsub_submit -G uboone --OS=SL6 --resource-provides=usage_model=opportunistic -N ${num_jobs} ${mail} file://${workdir}/RunGrid.sh -r ${filename}
echo
jobsub_submit -G uboone --OS=SL6 --resource-provides=usage_model=opportunistic -N ${num_jobs} ${mail} file://${workdir}/RunGrid.sh -r ${filename}

