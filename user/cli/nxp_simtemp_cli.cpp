#include <fcntl.h>
#include <poll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <cstdint>
#include <cstring>
#include <iostream>   
#include <sstream>
#include <string>
#include <map>
#include <vector>

static const std::string kDefaultBase = "/sys/class/misc/simtemp";
static const std::string kDefaultDev  = "/dev/simtemp";

struct __attribute__((packed)) SimtempSample 
{
  uint64_t timestamp_ns;
  int32_t  temp_mC;
  uint32_t flags;
};

static std::map<std::string,std::string> attrMap = {
    {"SAMP", "sampling_ms"},
    {"THRS", "threshold_mC"},
    {"MODE", "mode"},
    {"STAT", "state"}// RO
};

void printHelp() 
{
    std::cout << "Usage: nxp_simtempcli [COMMAND/OPTIONS] [ATT] ..." << std::endl;
    std::cout << "CLI program to be used alongs side the nxp_simtemp driver." << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  --help, -h   Display this help message and exit." << std::endl;
    std::cout << "\nCommands:" << std::endl;
    std::cout << "  poll, -p <#Samples>       \tWill wait for infomration for n Samples."         << std::endl;
    std::cout << "  get, -g  <ATT>      \tWill read the current value of the attribute."    << std::endl;
    std::cout << "  set, -s  <ATT> <Num>\tWill set a new value to the specified attribute." << std::endl;
    std::cout << "  Attributes (ATT, Permissions, Details):" << std::endl;
    std::cout << "    SAMP  - RW - Sample Time [ms]" << std::endl;
    std::cout << "    THRS  - RW - Threshold   [mC]" << std::endl;
    std::cout << "    MODE  - RW - [0-Normal, 1-Noisy, 2-RAMP]" << std::endl;
    std::cout << "    STAT  - RO - State [0-Normal, 1-Threshold Crossed]" << std::endl;
}

static std::string readFile(const std::string& path) 
{
  int fd = ::open(path.c_str(), O_RDONLY | O_CLOEXEC);
  if (fd < 0)
  {
    std::cerr << "Error: " << "Open 0" << path;
    if (errno) std::cerr << " (" << strerror(errno) << ")";
    std::cerr << "\n";
    _exit(2);
  }
  std::string out;
  char buf[256];
  for (;;)
  {
    ssize_t r = ::read(fd, buf, sizeof(buf));
    if (r < 0) 
    {
      int e = errno; 
      ::close(fd); 
      errno = e;
      std::cerr << "Error: " << "Open 1" << path;
      if (errno) std::cerr << " (" << strerror(errno) << ")";
      std::cerr << "\n";  
      _exit(2);
    }
    if (r == 0) break;
    out.append(buf, buf + r);
  }
  ::close(fd);
  // Trim trailing newline for nicer prints
  while (!out.empty() && (out.back() == '\n' || out.back() == '\r')) out.pop_back();
  return out;
}

int getAtt(std::string att)
{
  if(attrMap.find(att) == attrMap.end()) return -1;

  std::ostringstream oss;
  oss << kDefaultBase << "/" << attrMap[att];
  std::string attDir = oss.str();

  std::cout << attrMap[att] << ": " << readFile(oss.str()) << std::endl;
  return 0;
}

int main(int argc, char* argv[]) 
{
  std::vector<std::string> args(argv, argv + argc);

  if (argc > 1 && (args[1] == "help" || args[1] == "-h")) {
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
    if (args[1] == "set" || args[1] == "-s") 
    {
      if (argc < 4)
      {
        std::cerr << "Error: Missing argument for set." << std::endl;
        std::cerr << "Usage: " << args[0] << "set <attribute> <number>" << std::endl;
        return 1;
      }
    }
    else if (args[1] == "get" || args[1] == "-g") 
    {
      if (argc < 3)
      {
        std::cerr << "Error: Missing argument for get" << std::endl;
        std::cerr << "Usage: " << args[0] << " get <attribute>" << std::endl;
        return 1;
      }
      if(getAtt(args[2]))
      {
        std::cerr<< "Error: Wrong Attribute, verify attribute using help option."<<std::endl;
      }
    }
    else if (args[1] == "poll" || args[1] == "-p") 
    {
      if (argc < 3)
      {
        std::cerr << "Error: Missing argument for poll" << std::endl;
        std::cerr << "Usage: " << args[0] << " poll <#Samples>" << std::endl;
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
