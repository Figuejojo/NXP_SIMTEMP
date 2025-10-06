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
// cppcheck-suppress-end missingIncludeSystem

/***********************************************
 *  ENUMS and Sructs
 ***********************************************/
struct simtemp_device;

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
int nxp_simtemp_cdev_create(struct device *parent, struct simtemp_device **out);

/**
 * @brief Destroy /dev/simtemp and free resources.
 *
 * @param dev device object pointer.
 */
void nxp_simtemp_cdev_destroy(struct simtemp_device *dev);

/**
 * @brief Push new sampel and wake up poll.
 *
 * @param dev Ptr to the device object.
 * @param msg Message ready to be read.
 */
int nxp_simtemp_cdev_push_sample(struct simtemp_device *dev, const char *msg);

/**
 * @brief Set new threshold in mili-Celsius.
 *
 * @param dev Ptr to the device object.
 * @param time_ms Sample time in mili-Celsius.
 *
 * @return < 0 for any error code.
 */
int nxp_simtemp_cdev_set_threshold_mC(struct simtemp_device *dev, unsigned int th_mC);

/**
 * @brief Set new threshold in mili-Celsius.
 *
 * @param dev Ptr to the device object.
 *
 * @return sample in ms
 */
int nxp_simtemp_cdev_get_threshold_mC(struct simtemp_device *dev);

/**
 * @brief Set new sample time.
 *
 * @param dev Ptr to the device object.
 * @param time_ms Sample time in Ms.
 *
 * @return < 0 for any error code.
 */
int nxp_simtemp_cdev_set_sampling_ms(struct simtemp_device *dev, unsigned int time_ms);

/**
 * @brief Get current sample time.
 *
 * @param dev Ptr to the device object.
 *
 * @return sample in ms
 */
unsigned int nxp_simtemp_cdev_get_sampling_ms(struct simtemp_device *dev);

/**
 * @brief Set new mode.
 *
 * @param dev Ptr to the device object.
 * @param mode Mode to be set.
 *
 * @return < 0 for any error code.
 */
int nxp_simtemp_cdev_set_mode(struct simtemp_device *dev, simtemp_modes_e mode);

/**
 * @brief Get current mode.
 *
 * @param dev Ptr to the device object.
 *
 * @return current mode.
 */
unsigned int nxp_simtemp_cdev_get_mode(struct simtemp_device *dev);

/**
 * @brief Set current status.
 *
 * @param dev Ptr to the device object.
 *
 */
int nxp_simtemp_cdev_set_state(struct simtemp_device*dev, simtemp_state_e state);


/**
 * @brief Get current status.
 *
 * @param dev Ptr to the device object.
 *
 * @return current status.
 */
simtemp_state_e nxp_simtemp_cdev_get_state(struct simtemp_device*dev);

#endif //_NXP_SIMTEMP_HELPERS_CHARDEV_h_
