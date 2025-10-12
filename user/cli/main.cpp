/**
 * @file main.cpp
 * @brief Main entry point for nxp_simtemp CLI
 * @author Jose Jorge Figueroa
 * @author https://github.com/Figuejojo
 * @date 2025-10-08
 */

#include "nxp_simtemp_cli.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    NXPSimtempCLI cli;
    return cli.execute(argc, argv);
}