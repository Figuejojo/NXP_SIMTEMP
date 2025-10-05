/**
 * @file nxp_simtemp_helpers.h
 * @brief Header file for the nxp_siimtep_helpers.
 *
 * This file contains the declarations for helper functions.
 * Some of these helper functions are.
 * > Get time in iso8601 format.
 * > Generate temperature simulation.
 *
 * @author Jose Jorge Figueroa Figueroa
 * @author https://github.com/Figuejojo
 * @date 2025-10-1
 */
#ifndef _NXP_SIMTEMP_HELPERS_h_
#define _NXP_SIMTEMP_HELPERS_h_

 /***********************************************
 *  Linux Includes
 ***********************************************/
// cppcheck-suppress-begin missingIncludeSystem
#include <linux/timekeeping.h>
#include <linux/time64.h>
#include <linux/random.h> 
#include <linux/math.h>
// cppcheck-suppress-end missingIncludeSystem

 /***********************************************
 *  Function Declarations
 ***********************************************/
/**
 * @brief Generate a temperature value.
 *        Generation Modes:
 *          0 - NORMAL
 *          1 - NOISY
 *          2 - RAMP   
 * @return Temperature Value.
 */
long get_temperature_mC(simtemp_modes_e mode);

/**
 * @brief Get the current Date and time in iso8601 foramt.
 *        Format use for the timestmap.
 *
 * @param[out] buf String Ptr in ISO8601 time format.
 * @param[in]  buflen String length (Recommended 25).
 */
void ts_iso8601_now(char *buf, size_t buflen);

#endif // _NXP_SIMTEMP_HELPERS_h_