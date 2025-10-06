/**
 * @file nxp_simtemp_chardev.h
 * @brief Header file for the nxp_simtemp_chardev.
 *
 * @author Jose Jorge Figueroa Figueroa
 * @author https://github.com/Figuejojo
 * @date 2025-10-2
 */
#ifndef _NXP_SIMTEMP_HELPERS_CHARDEV_h_
#define _NXP_SIMTEMP_HELPERS_CHARDEV_h_

/***********************************************
 *  Linux Includes
 ***********************************************/
 // cppcheck-suppress-begin missingIncludeSystem
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/minmax.h>
#include <linux/poll.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/device.h>
#include <linux/types.h>
// cppcheck-suppress-end missingIncludeSystem

/***********************************************
 *  ENUMS and Sructs
 ***********************************************/
typedef struct simtemp_device
{
  struct miscdevice miscdev;
  struct device    *parent;

  /* Msg buffer returned on read (user-visible). */
  char   *msg;
  size_t  msg_len;

  /* Queue for reader waiting for data (poll)*/
  wait_queue_head_t wq;

  /* Synchronization */
  struct mutex lock;  // Protects msg during read/push
  u32 seq;            // Incremental on every push

  /* -- Sys Parameters -- */
  unsigned int sampling_ms;   /* Default 1000     */
  unsigned int threshold_mC;  /* Default 40000    */
  simtemp_modes_e mode;       /* Default 0-Normal */
  simtemp_state_e state ;     /* Default Okay     */

}simtemp_dev_t;

/***********************************************
 *  Function Declarations
 ***********************************************/
 /**
 * @brief Create /dev/simtemp.
 *
 * @param parent parent device (platform driver or class device struct).
 * @param out    device object pointer.
 *
 * @return Errors (EINVAL | ENOMEM | ...)
 */
int nxp_simtemp_cdev_create(struct device *parent, simtemp_dev_t **out);

/**
 * @brief Destroy /dev/simtemp and free resources.
 *
 * @param dev device object pointer.
 */
void nxp_simtemp_cdev_destroy(simtemp_dev_t *dev);

/**
 * @brief Push new sampel and wake up poll.
 *
 * @param dev Ptr to the device object.
 * @param msg Message ready to be read.
 */
int nxp_simtemp_cdev_push_sample(simtemp_dev_t *dev, const char *msg);

/**
 * @brief Set new threshold in mili-Celsius.
 *
 * @param dev Ptr to the device object.
 * @param time_ms Sample time in mili-Celsius.
 *
 * @return < 0 for any error code.
 */
int nxp_simtemp_cdev_set_threshold_mC(simtemp_dev_t *dev, unsigned int th_mC);

/**
 * @brief Set new threshold in mili-Celsius.
 *
 * @param dev Ptr to the device object.
 *
 * @return sample in ms
 */
int nxp_simtemp_cdev_get_threshold_mC(simtemp_dev_t *dev);

/**
 * @brief Set new sample time.
 *
 * @param dev Ptr to the device object.
 * @param time_ms Sample time in Ms.
 *
 * @return < 0 for any error code.
 */
int nxp_simtemp_cdev_set_sampling_ms(simtemp_dev_t *dev, unsigned int time_ms);

/**
 * @brief Get current sample time.
 *
 * @param dev Ptr to the device object.
 *
 * @return sample in ms
 */
unsigned int nxp_simtemp_cdev_get_sampling_ms(simtemp_dev_t *dev);

/**
 * @brief Set new mode.
 *
 * @param dev Ptr to the device object.
 * @param mode Mode to be set.
 *
 * @return < 0 for any error code.
 */
int nxp_simtemp_cdev_set_mode(simtemp_dev_t *dev, simtemp_modes_e mode);

/**
 * @brief Get current mode.
 *
 * @param dev Ptr to the device object.
 *
 * @return current mode.
 */
unsigned int nxp_simtemp_cdev_get_mode(simtemp_dev_t *dev);

/**
 * @brief Set current status.
 *
 * @param dev Ptr to the device object.
 *
 */
int nxp_simtemp_cdev_set_state(simtemp_dev_t*dev, simtemp_state_e state);


/**
 * @brief Get current status.
 *
 * @param dev Ptr to the device object.
 *
 * @return current status.
 */
simtemp_state_e nxp_simtemp_cdev_get_state(simtemp_dev_t*dev);

#endif //_NXP_SIMTEMP_HELPERS_CHARDEV_h_
