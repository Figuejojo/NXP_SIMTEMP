# TODO: Build and run steps.
These instructions are preliminary and are subject to change as the project evolves.

## Build procedure using the script
To build the module, navigate to the `scripts` directory and execute the `build.sh` script.

This script will perform the following steps:
1.  **Run linting:** It will run a linter to check C/C++ files. The process will be terminated if any linting errors are found.
2.  **Build the kernel driver:** It will build the kernel project to generate the required `.ko` driver file.
3.  **Install the driver:** It will install the driver using the `insmod` command.

# TODO: MUST include the links to the video and git repo