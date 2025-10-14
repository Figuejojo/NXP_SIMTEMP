#!/bin/bash

# Remember to give execution permissions
# chmod +x ./scripts/lint.sh

set -e

if [[ "$1" == "kernel" ]]; then
# Files to check
FILES_TO_CHECK=(
  "../kernel/nxp_simtemp_main.c"
  "../kernel/nxp_simtemp_helpers.c"
  "../kernel/nxp_simtemp_chardev.c"
)

# Run cppcheck with your desired options
# `--enable=all` activates all checks
# `-q` suppresses verbose progress messages
cppcheck --enable=all \
  "${FILES_TO_CHECK[@]}"  \
  --suppressions-list=suppressions.txt \
  --inline-suppr \
  --language=c --std=c17 \
  --error-exitcode=1 \

# Check the exit code of the previous command (cppcheck)
if [ $? -ne 0 ]; then
  echo "Cppcheck found issues. Build will fail."
  exit 1
else
  echo "Cppcheck passed. No issues found."
  # You can continue with your build process here
fi

elif [[ "$1" == "user" ]]; then

# Files to check
FILES_TO_CHECK_USER=(
  "../user/cli/main.cpp"
  "../user/cli/nxp_simtemp_cli.cpp"
)

# Run cppcheck with your desired options
# `--enable=all` activates all checks
# `-q` suppresses verbose progress messages
cppcheck --enable=all \
  "${FILES_TO_CHECK_USER[@]}"  \
  --suppressions-list=suppressions.txt \
  --inline-suppr \
  --language=c++ --std=c++17 \
  --error-exitcode=1 \

# Check the exit code of the previous command (cppcheck)
if [ $? -ne 0 ]; then
  echo "Cppcheck found issues. Build will fail."
  exit 1
else
  echo "Cppcheck passed. No issues found."
  # You can continue with your build process here
fi

else
  echo "Usage: $(basename "$0") {kernel|user}" >&2
  exit 2
fi
