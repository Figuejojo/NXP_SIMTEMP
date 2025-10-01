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
 /***********************************************
 *  Includes
 ***********************************************/
#include "nxp_simtemp.h"

/***********************************************
 *  Definitions
 ***********************************************/
#define DRV_VERSION "0.1.0"
#define DRV_NAME "NXP_simtmep"
#define DEF_SAMPLE_RATE_MS (1000)
/***********************************************
 *  Module Information
 ***********************************************/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jose Jorge Figueroa");
MODULE_DESCRIPTION("NXP Temperature Simulation Sensor");
MODULE_VERSION(DRV_VERSION);

/***********************************************
 *  Global Variables
 ***********************************************/
 unsigned int sampling_ms = DEF_SAMPLE_RATE_MS;

/***********************************************
 *  Static Function Prototypes
 ***********************************************/
static void nxp_simtemp_workfn(struct work_struct * work);
static int __init nxp_simtemp_init(void);
static void __exit nxp_simtemp_exit(void) ;

/***********************************************
 *  Static Variables
 ***********************************************/
static struct workqueue_struct *nxp_simtemp_wq;
static struct delayed_work      nxp_simtemp_work;

/***********************************************
 *  Functions
 ***********************************************/
 /**
 * @brief Work Function to handle the sensor simulation.
 *
 * This is the sensor simulatior, which will be executed every
 *  sampling_ms time.
 */
static void nxp_simtemp_workfn(struct work_struct * work)
{
    // TODO: Erase - Just for Development
    pr_info("[%s]: temp=20.5C",DRV_NAME);

    // Reschedule The Work Function.
    queue_delayed_work(nxp_simtemp_wq, &nxp_simtemp_work,
                        msecs_to_jiffies(max(1u, sampling_ms)));
}

/**
 * @brief Initializes the simulated temperature sensor module.
 *
 * This is the module's entry point, which registers the necessary
 * kernel structures.
 * @return >=0 Success
 * @return -ENOMEM - Memory issue when allocating memory.
 */
static int __init nxp_simtemp_init(void)
{
    printk(KERN_INFO "[%s] : Initializing Module\n",DRV_NAME);

    // Create a single-thread workqueue (Max 1 Thread)
    nxp_simtemp_wq = alloc_workqueue(DRV_NAME, WQ_UNBOUND | WQ_FREEZABLE, 1);
    if(!nxp_simtemp_wq)
    {
        // If there is a memory allocation error.
        pr_err("[%s]: Failed to create workqueue\n",DRV_NAME);
        return -ENOMEM;
    }

    INIT_DELAYED_WORK(&nxp_simtemp_work, nxp_simtemp_workfn);
    queue_delayed_work(nxp_simtemp_wq, &nxp_simtemp_work,
                        msecs_to_jiffies(max(1u,sampling_ms)));
    return 0;
}

/**
 * @brief Private exit function.
 *
 * This is the module's exit point. It unregisters resources, 
 * and does any closure as it is unloaded.
 */
static void __exit nxp_simtemp_exit(void)
{
    printk(KERN_INFO "[%s]: Unloading Module\n",DRV_NAME);
    if(nxp_simtemp_wq)
    {
        pr_info("[%s]: Exit - Closing Work Resources",DRV_NAME);
        cancel_delayed_work_sync(&nxp_simtemp_work);
        destroy_workqueue(nxp_simtemp_wq);
    }
}

module_init(nxp_simtemp_init);
module_exit(nxp_simtemp_exit);
