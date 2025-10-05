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
#include "nxp_simtemp_common.h"
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

  /* Queue for reader waiting for data (poll)*/
  wait_queue_head_t wq;

  /* Synchronization */
  struct mutex lock;  // Protects msg during read/push
  u32 seq;            // Incremental on every push

  unsigned int sampling_ms; /* Default 1000 */
  simtemp_modes_e mode;

}simtemp_dev_t;

static simtemp_dev_t *g_sdev;

// Pre-Open context for each File Descriptor
struct simtemp_file_ctx
{
  struct simtemp_device *dev;
  u32 seen_flag;
};

//Must match with elements in simtemp_modes_e
const char *mode_names[eEND] = {
                            "0 - Normal",
                            "1 - Noisy",
                            "2 - Ramp",
                          };
/***********************************************
 *  Static Function Prototypes
 ***********************************************/
static int simtemp_open(struct inode *inode, struct file *filp);
static ssize_t simtemp_read(struct file *filp, char __user *ubuf,
                            size_t count, loff_t *ppos);
static int simtemp_release(struct inode *inode, struct file *filp);
static __poll_t simtemp_poll(struct file *filp, poll_table *wait);
static int nxp_simtemp_cdev_set_message(struct simtemp_device *dev, const char *msg);

/***********************************************
 *  Static Functions
 ***********************************************/
 static const struct file_operations simtemp_fops = {
  .owner   = THIS_MODULE,
  .open    = simtemp_open,
  .release = simtemp_release,
  .read    = simtemp_read,
  .poll    = simtemp_poll,
  .llseek  = no_llseek,
};

/* ---------- file operations ---------- */
/**
 * @details Handle Openings.
 */
static int simtemp_open(struct inode *inode, struct file *filp)
{
  struct simtemp_file_ctx *ctx;

  if(!g_sdev) return -ENODEV;

  ctx = kzalloc(sizeof(*ctx), GFP_KERNEL);
  if(!ctx) return -ENOMEM;

  ctx->dev = g_sdev;

  {// Critical section
    mutex_lock(&g_sdev->lock);
    ctx->seen_flag = (READ_ONCE(g_sdev->seq) > 0) ? (READ_ONCE(g_sdev->seq) - 1) : 0;
    mutex_unlock(&g_sdev->lock);
  }

  filp->private_data = ctx;
  return 0;
}

/**
 * @details Handle Reading.
 */
static ssize_t simtemp_read(struct file *filp, char __user *ubuf,
                            size_t count, loff_t *ppos)
{
  struct simtemp_file_ctx *ctx = filp->private_data;
  struct simtemp_device *sdev;
  size_t to_copy;
  u32 this_seq;

  if(!ctx || !(sdev = ctx->dev)) return -ENODEV;

  if (ctx->seen_flag == READ_ONCE(sdev->seq))
  {
    if(filp->f_flags & O_NONBLOCK) return -EAGAIN;

    /* Sleep until seq changes (i.e., a push_sample happens). */
    if (wait_event_interruptible(sdev->wq, ctx->seen_flag != READ_ONCE(sdev->seq)))
    {
      return -ERESTARTSYS;
    }
  }

  { // Critical Section
    mutex_lock(&sdev->lock);
    this_seq = sdev->seq;

    // Reset Position for new sample
    if(*ppos > 0) *ppos = 0;

    if (*ppos >= sdev->msg_len)
    {
      // This fault is almost imposible.
      ctx->seen_flag = this_seq;
      mutex_unlock(&sdev->lock);
      return 0; // Return EOF
    }

    to_copy = min(count, sdev->msg_len - (size_t)*ppos);

    if (copy_to_user(ubuf, sdev->msg + *ppos, to_copy))
    {
      mutex_unlock(&sdev->lock);
      return -EFAULT;
    }

    *ppos+= to_copy;

    // Called has seen the message, mark as read.
    if(*ppos >= sdev->msg_len)
    {
      ctx->seen_flag = this_seq;
    }

    mutex_unlock(&sdev->lock);
  }

  *ppos += to_copy;
  return to_copy;
}

/**
 * @details Handle releases.
 */
static int simtemp_release(struct inode *inode, struct file *filp)
{
  kfree(filp->private_data);
  return 0;
}

/**
 * @details Handle Poll access.
 */
static __poll_t simtemp_poll(struct file *filp, poll_table *wait)
{
  struct simtemp_file_ctx *ctx = filp->private_data;
  struct simtemp_device *sdev;

  if(!ctx) return EPOLLERR;
  sdev = ctx->dev;

  poll_wait(filp, &sdev->wq, wait);

  if(READ_ONCE(sdev->seq) != ctx->seen_flag) return EPOLLIN | EPOLLRDNORM;

  return 0;
}
/* ---------- Sys Attribute Functions ---------- */
/* ---------- Sys/mode Functions ---------- */
// cppcheck-suppress-begin unusedFunction
static ssize_t mode_show(struct device *d,
                                struct device_attribute *attr, char *buf)
{
  //Mode Change this
  struct simtemp_device *dev = g_sdev;
  unsigned int mode = nxp_simtemp_chardev_get_mode(dev);
  return scnprintf(buf, PAGE_SIZE, "%s\n", mode_names[mode]);
}

static ssize_t mode_store(struct device *d,
                                 struct device_attribute *attr,
                                 const char *buf, size_t count)
{
  //MODE: Change this 
  struct simtemp_device *dev = g_sdev;
  unsigned int mode;

  int ret = kstrtouint(buf, 0, &mode);
  if (ret) return ret;

  ret = nxp_simtemp_chardev_set_mode(dev, (simtemp_modes_e)mode);
  if (ret) return ret;

  return count;
}
// cppcheck-suppress-end unusedFunction

static DEVICE_ATTR_RW(mode);


/* ---------- Sys/Sampling_ms Functions ---------- */
// cppcheck-suppress-begin unusedFunction
static ssize_t sampling_ms_show(struct device *d,
                                struct device_attribute *attr, char *buf)
{
  struct simtemp_device *dev = g_sdev;
  unsigned int time_ms = nxp_simtemp_chardev_get_sampling_ms(dev);
  return scnprintf(buf, PAGE_SIZE, "%u\n", time_ms);
}

static ssize_t sampling_ms_store(struct device *d,
                                 struct device_attribute *attr,
                                 const char *buf, size_t count)
{
  struct simtemp_device *dev = g_sdev;
  unsigned int ms;

  int ret = kstrtouint(buf, 0, &ms);
  if (ret) return ret;

  ret = nxp_simtemp_chardev_set_sampling_ms(dev, ms);
  if (ret) return ret;

  return count;
}
// cppcheck-suppress-end unusedFunction

static DEVICE_ATTR_RW(sampling_ms);

/* ---------- Message Functions ---------- */
/**
 * @details Set messages to be picked by /dev/ read access.
 */
static int nxp_simtemp_cdev_set_message(struct simtemp_device *dev, const char *msg)
{
  char *newbuf;
  size_t len;

  if (!dev || !msg) return -EINVAL;

  len = strlen(msg);
  newbuf = kmemdup_nul(msg, len, GFP_KERNEL);
  if (!newbuf)  return -ENOMEM;

  { // Critical Section
    mutex_lock(&dev->lock);
    kfree(dev->msg);
    dev->msg = newbuf;
    dev->msg_len = len;
    mutex_unlock(&dev->lock);
  }
  return 0;
}
/***********************************************
 *  Public Functions
 ***********************************************/
/**
 * @details Push Fresh sample and wake-up poll.
 */
int nxp_simtemp_cdev_push_sample(struct simtemp_device *dev, const char *msg)
{
  int ret;
  if(!dev) return -ENODEV;

  ret = nxp_simtemp_cdev_set_message(dev, msg);
  if(ret) return ret;

  {// Critical Section
    mutex_lock(&dev->lock);
    dev->seq++;
    mutex_unlock(&dev->lock);
  }
  // Notify Poll
  wake_up_interruptible(&dev->wq);
  return 0;
}

/**
 * @details Set sampling time in ms.
 */
int nxp_simtemp_chardev_set_sampling_ms(struct simtemp_device *dev, unsigned int time_ms)
{

  if (!dev) return -ENODEV;

  if (time_ms < MIN_SAMPLE_RATE_MS || time_ms > MAX_SAMPLE_RATE_MS) return -ERANGE;

  { // Critial Section
    mutex_lock(&dev->lock);
    dev->sampling_ms = time_ms;
    mutex_unlock(&dev->lock);
  }

  return 0;
}

/**
 * @details Get sampling time in ms.
 */
unsigned int nxp_simtemp_chardev_get_sampling_ms(struct simtemp_device *dev)
{
  unsigned int val = 0;
  if (!dev) return 0;

  { // Critical Section
    mutex_lock(&dev->lock);
    val = dev->sampling_ms;
    mutex_unlock(&dev->lock);
  }

  return val;
}

/**
 * @details Set temperature mode.
 */
int nxp_simtemp_chardev_set_mode(struct simtemp_device *dev, simtemp_modes_e mode)
{

  if (!dev) return -ENODEV;

  if (mode < eNORMAL || mode > eEND) return -ERANGE;

  { // Critial Section
    mutex_lock(&dev->lock);
    dev->mode = mode;
    mutex_unlock(&dev->lock);
  }

  return 0;
}

/**
 * @details Get temperature mode.
 */
unsigned int nxp_simtemp_chardev_get_mode(struct simtemp_device *dev)
{
  unsigned int val = 0;
  if (!dev) return 0;

  { // Critical Section
    mutex_lock(&dev->lock);
    val = (unsigned int)dev->mode;
    mutex_unlock(&dev->lock);
  }

  return val;
}

/**
 * @details Create the character device /dev/ object.
 */
int nxp_simtemp_cdev_create(struct device *parent, struct simtemp_device **out)
{
  int ret;
  struct simtemp_device *dev;

  if (!out) return -EINVAL;

  dev = kzalloc(sizeof(*dev), GFP_KERNEL);
  if (!dev) return -ENOMEM;

  /* Fill miscdevice fields: creates /dev/simtemp with dynamic minor. */
  dev->miscdev.minor = MISC_DYNAMIC_MINOR;
  dev->miscdev.name  = DRV_NAME;
  dev->miscdev.fops  = &simtemp_fops;
  /* Make it world-readable for easy testing; to be tighten later if needed. */
  dev->miscdev.mode  = 0444;
  dev->parent        = parent;

  mutex_init(&dev->lock);
  init_waitqueue_head(&dev->wq);
  dev->seq = 0;
  dev->sampling_ms = DEF_SAMPLE_RATE_MS;
  dev->mode = eNORMAL;

  ret = misc_register(&dev->miscdev);
  if (ret)
  {
    kfree(dev);
    return ret;
  }

  ret = device_create_file(dev->miscdev.this_device, &dev_attr_sampling_ms);
  if (ret)
  {
    misc_deregister(&dev->miscdev);
    kfree(dev);
    return ret;
  }

  ret = device_create_file(dev->miscdev.this_device, &dev_attr_mode);
  if (ret)
  {
    device_remove_file(dev->miscdev.this_device, &dev_attr_sampling_ms);
    misc_deregister(&dev->miscdev);
    kfree(dev);
    return ret;
  }


  /* Initial Value - Place Holder*/
  ret = nxp_simtemp_cdev_push_sample(dev, "2025-09-22T20:15:04.123Z temp=0 alert=0\n");
  if (ret)
  {
    device_remove_file(dev->miscdev.this_device, &dev_attr_sampling_ms);
    device_remove_file(dev->miscdev.this_device, &dev_attr_mode);
    misc_deregister(&dev->miscdev);
    kfree(dev);
    return ret;
  }

  g_sdev = dev;
  *out   = dev;

  pr_info("[%s]: /dev/%s registered (minor=%d)\n",DRV_NAME, DRV_NAME, dev->miscdev.minor);
  pr_info("[%s] sysfs: /sys/class/misc/%s/sampling_ms (default=%u ms)\n",
          DRV_NAME, DRV_NAME, dev->sampling_ms);

  return 0;
}

/**
 * @details Destroys the character device /dev/ object.
 */
void nxp_simtemp_cdev_destroy(struct simtemp_device *dev)
{
  if (!dev) return;

  device_remove_file(dev->miscdev.this_device, &dev_attr_sampling_ms);
  device_remove_file(dev->miscdev.this_device, &dev_attr_mode);

  misc_deregister(&dev->miscdev);
  kfree(dev->msg);
  kfree(dev);

  if (g_sdev == dev)  g_sdev = NULL;

  pr_info("[%s]: /dev/%s unregistered\n", DRV_NAME, DRV_NAME);
}
