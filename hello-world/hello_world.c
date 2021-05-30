/*
 * Hello World Kernel Module
 */

#include <linux/module.h>

static int __init hello_world_init(void) {
    pr_info("Hello world module loaded!\n");
    return 0;
}

static void __exit hello_world_exit(void) {
    pr_info("Hello world module unloaded. Bye!\n");
}

module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_AUTHOR("Suyash");
MODULE_DESCRIPTION("Hello world loadable kernel module");
MODULE_LICENSE("GPL");