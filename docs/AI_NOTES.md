# AI Notes
I experimented with AI assistants to produce clearer, more complete documentation for this project.

## Qemu Setup
While following the official QEMU instructions, I encountered several challenges getting the emulator to run.
Using AI to propose solutions, explain common usage patterns, and provide detailed clarifications helped me resolve these issues more quickly.
Some which:
- Headless emulator, as my laptop couldnt stand the emulator interface, using a terminal only interface was the solution.
- SSH setup so I can develop using VSCode.
- DNS fix, as the image did not had NetworkManager isntalled.

## Kernel Driver
- Searched for and reviewed resources focused on Linux kernel driver development.
- Requested a basic driver example to use as a reference alongside the resources above.
- Created a Gaussian random-number helper function with AI assistance.
- Provide a code to test the read, write and poll functions before the cli was finished.

## User CLI
- Iterated back-and-forth with AI to design the CLI’s UML until it met the project’s needs.
- Documented the setter/getter workflow for driver attributes.
- Requested a poll example to understand typical usage and to implement my own version.

## Resources
- [Linux Device Drivers](https://lwn.net/Kernel/LDD3/)
- [Linux Kernel](https://docs.kernel.org/index.html)