# Linux Kernel Module / Linux Device Drivers

## 1. [hello-world](/hello-world)
+ Simple hello world loadable kernel module(lkm).
+ linux/module.h includes linux init.h. Don't include it explicitly.
+ module.h contains module_init(), module_exit(), MODULE_AUTHOR and other 
    meta macros. 
+ check dmesg, lsmod for checking the module functionality.
+ Build using `make` and clean up using `make clean`
+ Load module using `sudo insmod hello_world.ko` and remove using 
    `sudo rmmod hello_world`

## 2. Hello World Character Driver
+ Simple character driver.
+ Device file: `/dev/hello_char_0`, sysfs class dir: `/sys/class/hello_char`
+ Change ownership of the device file `chown username /dev/dev/hello_char_0`
+ Reading `cat /dev/hello_char_0` or `strace cat /dev/hello_char_0`
+ Writing `echo "hello" > /dev/hello_char_0` or `strace echo "hello" > /dev/hello_char_0`
+ Use `demsg` for logs.