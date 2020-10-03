// A very basic platform driver, need to hook it up to a real LED and modify DTB
// Right now, it exposes /dev node, can read and write to userspace

#include <linux/fs.h>
#include <linux/mod_devicetable.h>
#include <linux/platform_device.h>
//#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/of.h>
//#include <linux/device.h>
#include <linux/miscdevice.h>

struct led_dev {
    struct miscdevice led_misc_device;
    const char* led_name;
    char led_value[8];
};

static ssize_t led_read(struct file * file, char __user * buff, size_t n, loff_t *ppos) {
    pr_info ("led_read(..) is called\n");
    struct led_dev* led_device = container_of(file->private_data, struct led_dev, led_misc_device);

//copy_to_user(void __user *to, const void *from, unsigned long n)
    if (*ppos == 0) {
        int len = strlen(led_device->led_value);
        pr_info("The size of the message is: %d\n", len);
        if (copy_to_user(buff, &led_device->led_value, len+1)) {
            pr_info("Failed to return led_value to user space\n");
            return -EFAULT;
        }
        *ppos += 1;
        return sizeof(led_device->led_value);
    }

    pr_info("Read completed\n");
    return 0;
}

static ssize_t led_write(struct file* file, const char __user* buff, size_t count, loff_t * ppos) {
    pr_info ("led_write(..) is called\n");
    struct led_dev* led_device = container_of(file->private_data, struct led_dev, led_misc_device);

//copy_from_user(void *to, const void __user *from, unsigned long n)
    if (copy_from_user(led_device->led_value, buff, count)) {
        pr_info("Bad copied value\n");
        return -EFAULT;
    }

    led_device->led_value[count-1] = '\0';
    pr_info("The message received from user space is: %s. Write compelte\n", led_device->led_value);
    return count;
}


static struct file_operations led_fops = {
    .owner = THIS_MODULE,
    .read = led_read,
    .write = led_write
};



static int __init led_probe(struct platform_device* pdev) {
    pr_info("led_probe() function is called\n");

    struct led_dev* led_device = devm_kzalloc(&pdev->dev, sizeof(struct led_dev), GFP_KERNEL);
	of_property_read_string(pdev->dev.of_node, "label", &led_device->led_name);

    led_device->led_misc_device.fops = &led_fops;
    led_device->led_misc_device.minor = MISC_DYNAMIC_MINOR;
    led_device->led_misc_device.name = led_device->led_name;

    char initial_val[] = "off\n";
    memcpy(led_device->led_value, initial_val, sizeof(initial_val));

    int ret = misc_register(&led_device->led_misc_device);

    if (ret < 0) {
        pr_info("Unable to allocate misc device  number\n");
        return ret;
    }

    platform_set_drvdata(pdev, led_device);

    pr_info("Device created successfully.\n");
    return 0;
}

static int __exit led_remove(struct platform_device* pdev) {
    pr_info("led_remove() function called\n");

    struct led_dev* led_device = platform_get_drvdata(pdev);
    misc_deregister(&led_device->led_misc_device);

    pr_info("led_remove() exiting\n");
    return 0;
}

static const struct of_device_id my_of_ids[] = {
    { .compatible = "arrow,RGBleds"},
    {}
};
MODULE_DEVICE_TABLE(of, my_of_ids);

static struct platform_driver my_platform_driver = {
    .probe = led_probe,
    .remove = led_remove,
    .driver = {
        .name = "RGBleds",
        .of_match_table = my_of_ids,
        .owner = THIS_MODULE
    }
};

module_platform_driver(my_platform_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hello author");
MODULE_DESCRIPTION("Learning how to control RGBleds");
