/**
 * @file nxp_simtemp.c
 * @brief Implementation for the nxp_simtemp.
 *
 * This source file provides the imeplementation for a simulated NXP Temperature
 *  Sensor.
 *
 * @author Jose Jorge Figueroa
 * @author https://github.com/Figuejojo
 * @date 2025-09-29
 */
#include "nxp_simtemp.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jose Jorge Figueroa");
MODULE_DESCRIPTION("NXP Temperature Simulation Sensor");

/**
 * @brief Initializes the simulated temperature sensor module.
 *
 * This is the module's entry point, which registers the necessary
 * kernel structures.
 * @return TBD.
 */
static int __init nxp_simtemp_init(void) {
    printk(KERN_INFO "Initializing NXP Temperature Sensor\n");
    return 0;
}

/**
 * @brief Private exit function.
 *
 * This is the module's exit point. It unregisters resources, 
 * and does any closure as it is unloaded.
 */
static void __exit nxp_simtemp_exit(void) {
    printk(KERN_INFO "Unloading NXP Temperature Sensor\n");
}

module_init(nxp_simtemp_init);
module_exit(nxp_simtemp_exit);
