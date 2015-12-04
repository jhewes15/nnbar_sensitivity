#!/bin/bash

# USAGE

usage="
Usage: $(basename "$0")

        -- Script to submit jobs which perform an nnbar limit calculation --

    OPTIONS:

        -h            Show this help page.
        -r            Run name (required)
        -p            Use custom parameters.
                        syntax: \"-p <efficiency>,<efficiency uncertainty,<exposure>,<exposure uncertainty>,
                                    <background>,<background uncertainty>,<number of observed events>\"
"

# Parse optional arguments.

run_stage=0

while getopts "hr:p:" option; do
  case "${option}" in
    h)  echo "${usage}"
        exit
        ;;
    r)  run_name=${OPTARG}
        ;;
    p)  params=${OPTARG}
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

echo run name is ${run_name}

# Sanity check on input arguments

if [ -z "${run_name}" ]; then
  echo "No run name specified!" >&2
  echo >&2
  echo "${usage}" >&2
  exit 1
fi

umask 002

export WORKDIR_HOME=$(pwd)
export OUTDIR=/uboone/data/users/jhewes15/limit

jobsub_submit -G uboone --OS=SL6 --resource-provides=usage_model=opportunistic -N 1000 -e WORKDIR_HOME -dOUT ${OUTDIR} file://${WORKDIR_HOME}/RunLimit.sh ${run_name} ${params}

