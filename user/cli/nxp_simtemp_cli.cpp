/**
 * @file nxp_simtemp_cli.cpp
 * @brief CLI integration for the simtemp driver.
 *
 *
 * @author Jose Jorge Figueroa
 * @author https://github.com/Figuejojo
 * @date 2025-10-08
 */
 /***********************************************
 *  Includes
 ***********************************************/
#include "nxp_simtemp_cli.hpp"

/***********************************************
 *  Const Variables
 ***********************************************/
const std::string NXPSimtempCLI::kDefaultBase = "/sys/class/misc/simtemp";
const std::string NXPSimtempCLI::kDefaultDev = "/dev/simtemp";

/***********************************************
 *  Static Variables
 ***********************************************/

/***********************************************
 *  Public Methods
 ***********************************************/
int NXPSimtempCLI::execute(int argc, char* argv[]) 
{
  std::vector<std::string> args(argv, argv + argc);

  if (argc < 2) 
  {
    std::cerr << "Error: No command provided." << std::endl;
    printHelp();
    return 1;
  }

  const std::string& command = args[1];

  if (command == "help" || command == "-h" || command == "--help") 
  {
    printHelp();
    return 0;
  }
  else if (command == "set" || command == "-s") 
  {
    return executeSetCommand(args);
  }
  else if (command == "get" || command == "-g") 
  {
    return executeGetCommand(args);
  }
  else if (command == "poll" || command == "-p") 
  {
    return executePollCommand(args);
  }
  else 
  {
    std::cerr << "Error: Unknown command '" << command << "'." << std::endl;
    printHelp();
    return 1;
  }
}

int NXPSimtempCLI::setAttribute(const std::string& att, const std::string& value)
{
  if (!isValidAttribute(att)) 
  {
    std::cerr << "Error: Invalid attribute '" << att << "'." << std::endl;
    return -1;
  }
  
  if (isAttributeReadOnly(att)) 
  {
    std::cerr << "Error: Attribute '" << att << "' is read-only." << std::endl;
    return -1;
  }

  std::string path = getAttributePath(att);
  writeFile(path, value + "\n");
  std::cout << att << " set to: " << value << std::endl;
  return 0;
}

int NXPSimtempCLI::getAttribute(const std::string& att) 
{
  if (!isValidAttribute(att)) 
  {
    std::cerr << "Error: Invalid attribute '" << att << "'." << std::endl;
    return -1;
  }

  std::string path = getAttributePath(att);
  std::string value = readFile(path);
  std::cout << att << ": " << value << std::endl;
  return 0;
}

int NXPSimtempCLI::pollSamples(int numSamples)
{
  // Implementation for polling samples
  std::cout << "Polling for " << numSamples << " samples..." << std::endl;
  // TODO: Implement actual polling logic
  return 0;
}


/***********************************************
 *  Private Methods
 ***********************************************/
void NXPSimtempCLI::printHelp() const 
{
  std::cout << "Usage: nxp_simtempcli [COMMAND] [OPTIONS] [ARGUMENTS]\n";
  std::cout << "CLI program for interacting with the nxp_simtemp driver.\n\n";
  
  std::cout << "COMMANDS:\n";
  std::cout << "  poll, -p <samples>      Poll for specified number of samples\n";
  std::cout << "  get,  -g <attribute>    Read current value of attribute\n";
  std::cout << "  set,  -s <attribute> <value>  Set attribute to new value\n\n";
  
  std::cout << "OPTIONS:\n";
  std::cout << "  --help, -h              Display this help message\n\n";
  
  std::cout << "ATTRIBUTES:\n";
  std::cout << "  SAMP  (RW)  Sample Time [ms]\n";
  std::cout << "  THRS  (RW)  Threshold [mC]\n";
  std::cout << "  MODE  (RW)  Mode: 0=Normal, 1=Noisy, 2=RAMP\n";
  std::cout << "  STAT  (RO)  State: 0=Normal, 1=Threshold Crossed\n";
}

void NXPSimtempCLI::handleError(const std::string& msg, bool useErrno) const 
{
  std::cerr << "Error: " << msg;
  if (useErrno && errno) 
  {
    std::cerr << " (" << strerror(errno) << ")";
  }
  std::cerr << std::endl;
  exit(2);
}

bool NXPSimtempCLI::isValidAttribute(const std::string& att) const 
{
  return attrMap.find(att) != attrMap.end();
}

std::string NXPSimtempCLI::getAttributePath(const std::string& att) const 
{
  std::ostringstream oss;
  oss << kDefaultBase << "/" << attrMap.at(att);
  return oss.str();
}

bool NXPSimtempCLI::isAttributeReadOnly(const std::string& att) const 
{
  return (att == "STAT");
}

std::string NXPSimtempCLI::readFile(const std::string& path) const 
{
  int fd = ::open(path.c_str(), O_RDONLY | O_CLOEXEC);
  if (fd < 0)  handleError("Open (read): " + path);

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
      handleError("read: " + path);
    }
    if (r == 0) break;
    out.append(buf, buf + r);
  }

  ::close(fd);
  
  // Trim trailing newline for cleaner output
  while (!out.empty() && (out.back() == '\n' || out.back() == '\r')) 
  {
    out.pop_back();
  }
  
  return out;
}

void NXPSimtempCLI::writeFile(const std::string& path, const std::string& value) const 
{
  int fd = ::open(path.c_str(), O_WRONLY | O_CLOEXEC);
  if (fd < 0) 
  {
    handleError("open (write): " + path);
  }

  const char* ptr_val = value.c_str();
  size_t bytesleft = value.size();

  while (bytesleft) 
  {
    ssize_t bytesWritten = ::write(fd, ptr_val, bytesleft);
    if (bytesWritten < 0)
    {
      int e = errno;
      ::close(fd);
      errno = e;
      handleError("write: " + path);
    }
    bytesleft -= (size_t)bytesWritten;
    ptr_val += bytesWritten;
  }
  ::close(fd);
}

int NXPSimtempCLI::executeSetCommand(const std::vector<std::string>& args) 
{
  if (args.size() < 4) 
  {
    std::cerr << "Error: Missing arguments for set command." << std::endl;
    std::cerr << "Usage: " << args[0] << " set <attribute> <value>" << std::endl;
    return 1;
  }

  const std::string& att = args[2];
  const std::string& value = args[3];

  if (!isValidAttribute(att))
  {
    std::cerr << "Error: Invalid attribute '" << att << "'." << std::endl;
    std::cerr << "Valid attributes: SAMP, THRS, MODE, STAT" << std::endl;
    return 1;
  }

  return setAttribute(att, value);
}

int NXPSimtempCLI::executeGetCommand(const std::vector<std::string>& args) 
{
    if (args.size() < 3) 
    {
        std::cerr << "Error: Missing argument for get command." << std::endl;
        std::cerr << "Usage: " << args[0] << " get <attribute>" << std::endl;
        return 1;
    }

    const std::string& att = args[2];

    if (!isValidAttribute(att)) 
    {
        std::cerr << "Error: Invalid attribute '" << att << "'." << std::endl;
        std::cerr << "Valid attributes: SAMP, THRS, MODE, STAT" << std::endl;
        return 1;
    }

    return getAttribute(att);
}

int NXPSimtempCLI::executePollCommand(const std::vector<std::string>& args) 
{
  if (args.size() < 3) 
  {
    std::cerr << "Error: Missing argument for poll command." << std::endl;
    std::cerr << "Usage: " << args[0] << " poll <samples>" << std::endl;
    return 1;
  }

  try 
  {
    int numSamples = std::stoi(args[2]);
    return pollSamples(numSamples);
  } 
  catch (const std::exception& e) 
  {
    std::cerr << "Error: Invalid number of samples: " << args[2] << std::endl;
    return 1;
  }
}