#!/usr/bin/env bash
export LC_ALL="C"

# check output against reference
# to have minimal regression test
${ALICE_ROOT}/test/stripLog.sh sim.log > sim.log_cleaned
diff sim.log_cleaned sim.log_reference > sim.log_diff
SIMCODE=$?

${ALICE_ROOT}/test/stripLog.sh rec.log > rec.log_cleaned
diff rec.log_cleaned rec.log_reference > rec.log_diff
RECCODE=$?

${ALICE_ROOT}/test/stripLog.sh check.log > check.log_cleaned
diff check.log_cleaned check.log_reference > check.log_diff
CHECKCODE=$?


# color codes for output
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m' #no color
if [ "${SIMCODE}" = "0" ]; then
  echo -e "${BLUE}SIMULATION REGRESSION PASSED${NC}";
else
  echo -e "${RED}SIMULATION REGRESSION (POSSIBLY) FAILED${NC}";
fi 
if [ "${RECCODE}" = "0" ]; then
  echo -e "${BLUE}REC REGRESSION PASSED${NC}";
else
  echo -e "${RED}REC REGRESSION (POSSIBLY) FAILED${NC}";
fi 
if [ "${CHECKCODE}" = "0" ]; then
  echo -e "${BLUE}CHECKESD REGRESSION PASSED${NC}";
else
  echo -e "${RED}CHECKESD REGRESSION (POSSIBLY) FAILED${NC}";
fi 

# in future exit with some code
exit ${CHECKCODE}
