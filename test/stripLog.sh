#!/usr/bin/env bash 
set -e

# A script that removes dynamic information from log files
# with the purpose to compare them to a reference (and hence check
# correctness of output in regression tests)
# The script is by far not complete and will "learn" to filter the right things over time

input=$1

# strip dates
cat ${input} |\
    sed -e 's/\(Jan\|Feb\|Mar\|Apr\|May\|Jun\|Jul\|Aug\|Sep\|Oct\|Nov\|Dec\)\s[0-9][0-9]\s20[0-9][0-9]/DATE/g' | \
    sed -e 's/[0-9][0-9]:[0-9][0-9]:[0-9][0-9]/TIME/g' |\
    sed -e 's/0x[0-9a-fA-F][0-9a-fA-F]*/POINTER/g'  |\
    perl -p -e 's|/[^\s/][^\s/]*|PATH|g' |\
    perl -p -e 's|(PATH)+|/DIR|g' |\
    perl -p -e 's|[CR]:[0-9]+\.[0-9]+s|TIMING|g' |\
    perl -p -e 's/[0-9]+\.[0-9]+\s+[Kb|MB]/MEMORY/g' |\
    grep -v 'LD_PRELOAD' |\
    grep -v 'Real time' |\
    grep -v 'Time' |\
    grep -v 'ZEBRA table base' |\
    grep -v 'absolute adrs' |\
    grep -v 'relative adrs' |\
    grep -v 'HEX' |\
    grep -v -e '\*.*Version'  |\
    grep -v 'CINT' |\
    grep -v 'ROOT' |\
    grep -v 'GEANT Version' |\
    grep -v 'Correction Cradle'


# CONSIDER FURTHER:
# VERSION NUMBERS (ROOT; GEANT3...)
# ZEBRA ADDRESSES

