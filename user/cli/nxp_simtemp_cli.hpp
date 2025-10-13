/**
 * @file nxp_simtemp_cli.h
 * @brief CLI class for interacting with the simtemp driver
 * @author Jose Jorge Figueroa
 * @author https://github.com/Figuejojo
 * @date 2025-10-08
 */

#ifndef _NXP_SIMTEMP_CLI_HPP_
#define _NXP_SIMTEMP_CLI_HPP_

// cppcheck-suppress-begin missingIncludeSystem
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
#include <iomanip>
#include <chrono>
// cppcheck-suppress-end missingIncludeSystem

struct SimtempSample 
{
    uint64_t timestamp_ns;
    int32_t  temp_mC;
    uint32_t flags;
} __attribute__((packed));

class NXPSimtempCLI 
{
  public:

    NXPSimtempCLI()  = default;
    ~NXPSimtempCLI() = default;

    // Main command execution
    int execute(int argc, char* argv[]);

    // Command methods
    int setAttribute(const std::string& att, const std::string& value);
    int getAttribute(const std::string& att);
    int pollSamples(int numSamples);

  private:
    static const std::string kDefaultBase;
    static const std::string kDefaultDev;

    // Help Method
    void printHelp() const;

    //Attribute map
    std::map<std::string, std::string> attrMap{
      {"SAMP", "sampling_ms"},
      {"THRS", "threshold_mC"},
      {"MODE",  "mode"},
      {"STAT",  "state"}
    };

    // File I/O methods
    std::string readFile(const std::string& path) const;
    void writeFile(const std::string& path, const std::string& value) const;
    
    // Path construction
    std::string getAttributePath(const std::string& att) const;
    
    // Error handling
    void handleError(const std::string& message, bool useErrno = true) const;

    // Attribute validation
    bool isValidAttribute(const std::string& att) const;
    bool isAttributeReadOnly(const std::string& att) const;

    // Command parsing helpers
    int executeSetCommand(const std::vector<std::string>& args);
    int executeGetCommand(const std::vector<std::string>& args);
    int executePollCommand(const std::vector<std::string>& args);
};

#endif //_NXP_SIMTEMP_CLI_HPP_