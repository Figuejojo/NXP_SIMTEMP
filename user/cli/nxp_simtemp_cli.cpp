#include <iostream>
#include <string>
#include <vector>



void printHelp() 
{
    std::cout << "Usage: nxp_simtempcli [OPTIONS] [COMMAND]" << std::endl;
    std::cout << "A simple CLI program to greet a user." << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  --help, -h   Display this help message and exit." << std::endl;
    std::cout << "\nCommands:" << std::endl;
    std::cout << "  --poll, -p <#Samples>\tWill pull infomration for n amount of Samples." << std::endl;
    std::cout << "  --read, -r <Attribute>\tWill read the currecnt value of the attribute." << std::endl;
    std::cout << "  --write, -w <Attribute> <Num>\tWill set a new value to the specified attribute." << std::endl;
    std::cout << "  Attributes (Cmd, Permissions, Details):" << std::endl;
    std::cout << "    MS   - RW - Sample Time [ms]" << std::endl;
    std::cout << "    THR  - RW - Threshold   [mC]" << std::endl;
    std::cout << "    MODE - RW - [0-Normal, 1-Noisy, 2-RAMP]" << std::endl;
    std::cout << "    ST   - RO - State [0-Normal, 1-TH Crossed]" << std::endl;
}

int main(int argc, char* argv[]) 
{
  std::vector<std::string> args(argv, argv + argc);

  if (argc > 1 && (args[1] == "--help" || args[1] == "-h")) {
      printHelp();
      return 0; // Exit successfully after printing help
  }

  if (argc < 2) 
  {
      std::cerr << "Error: No command provided." << std::endl;
      return 1; // Return a non-zero exit code for error
  }
  else
  {
    if (args[1] == "--write" || args[1] == "-w") 
    {
      if (argc < 4)
      {
        std::cerr << "Error: Missing argument for --write." << std::endl;
        std::cerr << "Usage: " << args[0] << " --write <attribute> <number>" << std::endl;
        return 1;
      }
    }
    else if (args[1] == "--read" || args[1] == "-r") 
    {
      if (argc < 3)
      {
        std::cerr << "Error: Missing argument for --read" << std::endl;
        std::cerr << "Usage: " << args[0] << " --read <attribute>" << std::endl;
        return 1;
      }
    }
    else if (args[1] == "--poll" || args[1] == "-p") 
    {
      if (argc < 3)
      {
        std::cerr << "Error: Missing argument for --poll" << std::endl;
        std::cerr << "Usage: " << args[0] << " --poll <#Samples>" << std::endl;
        return 1;
      }
    }
    else
    {
      std::cerr << "Error: Unknown command '" << args[1] << "'." << std::endl;
      return 1;
    }
  }
}
