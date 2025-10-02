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
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/minmax.h>



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
 * @brief Update the message to be returned by reads.
 *
 * @param dev Ptr to the device object.
 * @param msg Message ready to be read.
 */
int nxp_simtemp_cdev_set_message(struct simtemp_device *dev, const char *msg);

#endif //_NXP_SIMTEMP_HELPERS_CHARDEV_h_
