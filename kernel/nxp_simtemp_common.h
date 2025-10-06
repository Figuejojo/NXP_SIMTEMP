/**
 * @file nxp_simtemp_common.h
 * @brief Common header file for the nxp_simtep.
 *
 *
 * @author Jose Jorge Figueroa Figueroa
 * @author https://github.com/Figuejojo
 * @date 2025-09-29
 */
#ifndef _NXP_SIMTEMP_H_
#define _NXP_SIMTEMP_H_

/***********************************************
*  Linux Includes
***********************************************/
// cppcheck-suppress-begin missingIncludeSystem
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/workqueue.h>
#include <linux/jiffies.h>
#include <linux/types.h>
// cppcheck-suppress-end missingIncludeSystem


/***********************************************
 *  Definitions
 ***********************************************/
/* --- Common Driver Definitions --- */
#define DRV_NAME "simtemp"
#define DRV_VERSION "0.1.7"

/* --- Sample Time Driver Definitions --- */
#define DEF_SAMPLE_RATE_MS (30000)
#define MAX_SAMPLE_RATE_MS (3600000)
#define MIN_SAMPLE_RATE_MS (1)

/* --- Threshold Parameter Definitions --- */
#define DEF_THRESHOLD_mC   (35000)

/***********************************************
 *  ENUMS and Sructs
 ***********************************************/
typedef enum
{
  eNORMAL = 0,
  eNOISY  = 1,
  eRAMP   = 2,
  eEND
}simtemp_modes_e;

typedef enum
{
  eST_ERROR  = -1,
  eST_NORMAL = 0,
  eST_THRESH = 1,
  eST_END
}simtemp_state_e;

#endif //_NXP_SIMTEMP_H_