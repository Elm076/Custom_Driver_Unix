#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>


#define DRIVER_NAME "ECCDriver"
#define DRIVER_CLASS "ECCDriverClass"
#define NUM_DEVICES 1 /* Número de dispositivos a crear */
#define ALLOWED_UID 1000
#define ALLOWED_GID 1000
MODULE_LICENSE("GPL"); 


static dev_t major_minor = -1;
static struct cdev ECCcdev[NUM_DEVICES];
static struct class *ECCclass = NULL;
static char kernel_buffer[256];

static const struct file_operations ECC_fops;

static int __init init_driver(void) {

    int n_device;
    dev_t id_device;
    if (alloc_chrdev_region(&major_minor, 0, NUM_DEVICES, DRIVER_NAME) < 0) 
    {
        pr_err("Major number assignment failed");
        goto error;
    }

    pr_info("%s driver assigned %d major number\n", DRIVER_NAME, MAJOR(major_minor));

    //MODIFICACION DE LA IMPLEMENTACION DE LA FUNCION EN LA LIBR
    //if((ECCclass = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
    if((ECCclass = class_create(DRIVER_CLASS)) == NULL) {
        pr_err("Class device registering failed");
        goto error;
    }

    pr_info("/sys/class/%s class driver registered\n", DRIVER_CLASS);


    for (n_device = 0; n_device < NUM_DEVICES; n_device++) 
    {
        cdev_init(&ECCcdev[n_device], &ECC_fops);

        id_device = MKDEV(MAJOR(major_minor), MINOR(major_minor) + n_device);
        if(cdev_add(&ECCcdev[n_device], id_device, 1) == -1) {
            pr_err("Device node creation failed");
            goto error;
        }

        if(device_create(ECCclass, NULL, id_device, NULL, DRIVER_NAME "%d", n_device) == NULL) {
            pr_err("Device node creation failed");
            goto error;
        }

        pr_info("Device node /dev/%s%d created\n", DRIVER_NAME, n_device);
    }


    pr_info("ECC driver initialized and loaded\n");
    return 0;

error:
    if(ECCclass)
        class_destroy(ECCclass);

    if(major_minor != -1)
        unregister_chrdev_region(major_minor, NUM_DEVICES);

    return -1;
}


static int ECCopen(struct inode *inode, struct file *file) {
    pr_info("Device Opened");
    return 0;
}

static ssize_t ECCread(struct file *file, char __user *buffer, size_t count, loff_t *f_pos) {
    if (current_uid().val != ALLOWED_UID || current_gid().val != ALLOWED_GID)
    {
        pr_info("--CUIDADO-- Intento de LECTURA del dispositivo por usuario NO AUTORIZADO con UID: %d",current_uid().val);
        return -EACCES;
    }

    ssize_t bytes_to_copy;

    bytes_to_copy = min(count, sizeof(kernel_buffer) - *f_pos);

    if (copy_to_user(buffer, kernel_buffer + *f_pos, bytes_to_copy))
        return -EFAULT;
    
    *f_pos += bytes_to_copy;
    pr_info("LECTURA del dispositivo por usuario autentificado");

    return bytes_to_copy;
}



static ssize_t ECCwrite(struct file *file, const char __user *buffer, size_t count, loff_t *f_pos) {
    if (current_uid().val != ALLOWED_UID || current_gid().val != ALLOWED_GID)
    {
        pr_info("--CUIDADO-- Intento de ESCRITURA del dispositivo por usuario NO AUTORIZADO con UID: %d",current_uid().val);
        return -EACCES;
    }

    ssize_t bytes_not_copied;

    count = min(count, sizeof(kernel_buffer));
    
    bytes_not_copied = copy_from_user(kernel_buffer,buffer,count);

    if (bytes_not_copied) 
        return -EFAULT; 
    
    *f_pos += count - bytes_not_copied;

    pr_info("ESCRITURA del dispositivo por usuario autentificado");

    return count;
}

static int ECCrelease(struct inode *inode, struct file *file) {
    pr_info("Device Released");
    return 0;
}

static void __exit exit_driver(void) {
    int n_device;
    
    for (n_device = 0; n_device < NUM_DEVICES; n_device++) 
    {
        device_destroy(ECCclass, MKDEV(MAJOR(major_minor), MINOR(major_minor) + n_device));
        cdev_del(&ECCcdev[n_device]);
    }
    
    class_destroy(ECCclass);
    
    unregister_chrdev_region(major_minor, NUM_DEVICES);
    
    pr_info("ECC driver unloaded\n");
}

static const struct file_operations ECC_fops = {
    .owner = THIS_MODULE,
    .open = ECCopen,
    .read = ECCread,
    .write = ECCwrite,
    .release = ECCrelease,
};

module_init(init_driver)
module_exit(exit_driver)
