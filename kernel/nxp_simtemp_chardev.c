/**
 * @file nxp_simtemp_sys.c
 * @brief Implementation for the nxp_simtemp_sys.
 *
 *
 * @author Jose Jorge Figueroa
 * @author https://github.com/Figuejojo
 * @date 2025-09-29
 */
 /***********************************************
 *  Includes
 ***********************************************/
#include "nxp_simtemp_chardev.h"

/***********************************************
 *  Definitions
 ***********************************************/

/***********************************************
 *  Global Variables
 ***********************************************/

typedef struct simtemp_device 
{
  struct miscdevice miscdev;
  struct device    *parent;

  /* Msg buffer returned on read (user-visible). */
  char   *msg;
  size_t  msg_len;
}simtemp_dev_t;

static simtemp_dev_t *g_sdev;
/***********************************************
 *  Static Function Prototypes
 ***********************************************/

/***********************************************
 *  Static Functions
 ***********************************************/
/* ---------- file operations ---------- */
static int simtemp_open(struct inode *inode, struct file *filp)
{
  filp->private_data = g_sdev;
  return 0;
}

static ssize_t simtemp_read(struct file *filp, char __user *ubuf,
                            size_t count, loff_t *ppos)
{
  struct simtemp_device *sdev = filp->private_data ? : g_sdev;
  size_t remaining, to_copy;

  if (!sdev || !sdev->msg)  return -ENODEV;

  if (*ppos >= sdev->msg_len) return 0;

  remaining = sdev->msg_len - *ppos;
  to_copy   = min(count, remaining);

  if (copy_to_user(ubuf, sdev->msg + *ppos, to_copy)) return -EFAULT;

  *ppos += to_copy;
  return to_copy;
}

static const struct file_operations simtemp_fops = {
  .owner = THIS_MODULE,
  .open  = simtemp_open,
  .read  = simtemp_read,
  .llseek = no_llseek,
};

/***********************************************
 *  Public Functions
 ***********************************************/
 /**
 * @details Set messages to be picket by /dev/ read access.
 */
int nxp_simtemp_cdev_set_message(struct simtemp_device *dev, const char *msg)
{
  char *newbuf;
  size_t len;

  if (!dev || !msg) return -EINVAL;

  len = strlen(msg);
  newbuf = kmemdup_nul(msg, len, GFP_KERNEL);
  if (!newbuf)  return -ENOMEM;

  kfree(dev->msg);
  dev->msg = newbuf;
  dev->msg_len = len;
  return 0;
}

/**
 * @details Create the character device /dev/ object.
 */
int nxp_simtemp_cdev_create(struct device *parent, struct simtemp_device **out)
{
  int ret;
  struct simtemp_device *dev;

  if (!out)
    return -EINVAL;

  dev = kzalloc(sizeof(*dev), GFP_KERNEL);
  if (!dev)
    return -ENOMEM;

  /* Fill miscdevice fields: creates /dev/simtemp with dynamic minor. */
  dev->miscdev.minor = MISC_DYNAMIC_MINOR;
  dev->miscdev.name  = "simtemp";
  dev->miscdev.fops  = &simtemp_fops;
  /* Make it world-readable for easy testing; to be tighten later if needed. */
  dev->miscdev.mode  = 0444;
  dev->parent        = parent;

  ret = misc_register(&dev->miscdev);
  if (ret) {
    kfree(dev);
    return ret;
  }

  /* Initial Value - Place Holder*/
  ret = nxp_simtemp_cdev_set_message(dev, "2025-09-22T20:15:04.123Z temp=0 alert=0\n");
  if (ret) {
    misc_deregister(&dev->miscdev);
    kfree(dev);
    return ret;
  }

  g_sdev = dev;
  *out   = dev;

  pr_info("[simtemp]: /dev/simtemp registered (minor=%d)\n", dev->miscdev.minor);
  return 0;
}

/**
 * @details Destroys the character device /dev/ object.
 */
void nxp_simtemp_cdev_destroy(struct simtemp_device *dev)
{
  if (!dev) return;

  misc_deregister(&dev->miscdev);
  kfree(dev->msg);
  kfree(dev);

  if (g_sdev == dev)  g_sdev = NULL;

  pr_info("[simtemp]: /dev/simtemp unregistered\n");
}
