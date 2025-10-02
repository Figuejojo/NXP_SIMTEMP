#!/bin/bash

# Remember to give execution permissions
# chmod +x ./scripts/lint.sh

# Files to check
FILES_TO_CHECK=(
  "../kernel/nxp_simtemp_main.c"
  "../kernel/nxp_simtemp_helpers.c"
)

# Run cppcheck with your desired options
# `--enable=all` activates all checks
# `-q` suppresses verbose progress messages
cppcheck --enable=all \
  "${FILES_TO_CHECK[@]}"  \
  --suppressions-list=suppressions.txt \
  --error-exitcode=1

# Check the exit code of the previous command (cppcheck)
if [ $? -ne 0 ]; then
  echo "Cppcheck found issues. Build will fail."
  exit 1
else
  echo "Cppcheck passed. No issues found."
  # You can continue with your build process here
fi