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
#include "nxp_simtemp_main.h"
#include "nxp_simtemp_helpers.h"
#include "nxp_simtemp_chardev.h"

/***********************************************
 *  Definitions
 ***********************************************/
#define DRV_VERSION "0.1.0"
#define DRV_NAME "simtmep"
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
static struct simtemp_device *g_simtemp_dev;
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

    long temp_mC = get_normal_temperature_mC();
    
    char buf[50];
    snprintf(buf, sizeof(buf), "2025-09-22T20:15:04.123Z temp=%ld.%ldC alert=0\n",
             (temp_mC / 1000), (temp_mC % 1000));

    if(0 < nxp_simtemp_cdev_set_message(g_simtemp_dev, buf))
    {
      pr_info("[%s]: Temp: Failed to Update\n",DRV_NAME);
    }
    pr_info("[%s]: Temp: Updated\n",DRV_NAME);

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
    int ret;
    printk(KERN_INFO "[%s]: Initializing Module\n",DRV_NAME);

    ret = nxp_simtemp_cdev_create(NULL, &g_simtemp_dev);
	if (ret)
    {
		pr_err("[%s] failed to create /dev/simtemp: %d\n",DRV_NAME, ret);
		return ret;
	}

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

    nxp_simtemp_cdev_destroy(g_simtemp_dev);

    if(nxp_simtemp_wq)
    {
        printk("[%s]: Exit - Closing Work Resources",DRV_NAME);
        cancel_delayed_work_sync(&nxp_simtemp_work);
        destroy_workqueue(nxp_simtemp_wq);
    }

}

module_init(nxp_simtemp_init);
module_exit(nxp_simtemp_exit);
