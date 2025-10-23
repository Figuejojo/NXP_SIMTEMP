# Build and run steps.

## Build procedure using the script
To build the module, navigate to the `scripts` directory and execute the `build.sh` script.
This script will perform the following steps:
1. **Run lint on kernel folder:** Execute ```./lint.sh kernel```, which runs a static code analysis over the kernel folder. These are checked using ```cppcheck``` for C language. The process will be terminated if any errors are found.
2. **Build and Install Driver:** Build the kernel project to generate the required `.ko` driver file. Following this, if the driver was built correctly, it will be installed using ```insmod nxp_simtemp.ko```; otherwise the process will be stopped.
3. **Run lint on user/cli folder:** Execute ```./lint.sh user```. ```cppcheck``` will run a static code analysis, but for C++ under the user folder. If it fails, the process is terminated.
4. **Build User:** Build the user applications. CLI and GUI. The executable is then stored under its respective build folder. I.e: ./user/cli/build/[exe].

## Run Steps
Once build and installed, use the cli executable to interact with the driver.
```nxp_simtemp_cli```supports the following commands:
- ```-h``` or ```help```: Detail all the CLI capabilities.
- ```-p``` or ```poll```: Wait to poll new data or alarm data, and format the binary data.
- ```-s``` oe ```set```: Write new data to a driver attributes.
- ```-g``` oe ```get```: Read current data from a driver attributes.
List of supported attributes:
- ```SAMP```: Sampling Time, measured in ms, with RW permissions. (Default: 30,000ms)
- ```MODE```: Sampling MODE, three Sensor Modes, with RW permissions. (Default: 0-Normal)
- ```THRS```: Temp Threshold, measured in mC, with RW permissions. (Default: 35000mC)
- ```STAT```: Sensor Status, two states, with R permissions. (Default: 0: Normal)
For more detail info on the attributes and commands, use the cli executable with the help option.

# LINKS
- Github Repo: [NXP SIMTEMP Driver](https://github.com/Figuejojo/NXP_SIMTEM)
- Usage Video: [Walkthrough video](https://drive.google.com/file/d/1uo8o_3Zb-gIUaV_5jhqZJjyFcyJsKiaE/view?usp=sharing)
