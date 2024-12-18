#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/ktime.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Hello, world with BUG_ON and simulated kmalloc failure");
MODULE_AUTHOR("Prokhorenko Artem, IO-23");

static uint count = 1;
module_param(count, uint, 0444);
MODULE_PARM_DESC(count, "Number of times to print 'Hello, world!'");

struct time_data {
    struct list_head list;
    ktime_t time_before;
    ktime_t time_after;
};

static LIST_HEAD(time_list);

static int __init hello_init(void) {
    struct time_data *data;
    uint i;

    pr_info("Module hello loaded with count=%u\n", count);

    BUG_ON(count > 10);

    for (i = 0; i < count; i++) {
        data = kmalloc(sizeof(*data), GFP_KERNEL);

        if (!data && i == count - 1) {
            BUG_ON(!data);
        }

        if (data) {
            data->time_before = ktime_get();
            pr_info("Hello, world! %u/%u\n", i + 1, count);
            data->time_after = ktime_get();
            list_add_tail(&data->list, &time_list);
        }
    }
    return 0;
}

static void __exit hello_exit(void) {
    struct time_data *data, *tmp;
    pr_info("Unloading hello module. Printing times:\n");

    list_for_each_entry_safe(data, tmp, &time_list, list) {
        pr_info("Time spent: %lld ns\n",
                ktime_to_ns(ktime_sub(data->time_after, data->time_before)));
        list_del(&data->list);
        kfree(data);
    }
}

module_init(hello_init);
module_exit(hello_exit);
