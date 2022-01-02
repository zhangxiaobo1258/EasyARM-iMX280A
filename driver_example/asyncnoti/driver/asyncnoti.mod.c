#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x65107f0, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xa5a01498, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x42e83abd, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0x269302bc, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0xfe990052, __VMLINUX_SYMBOL_STR(gpio_free) },
	{ 0xc1514a3b, __VMLINUX_SYMBOL_STR(free_irq) },
	{ 0xd211501, __VMLINUX_SYMBOL_STR(del_timer) },
	{ 0xca9a05c5, __VMLINUX_SYMBOL_STR(__init_waitqueue_head) },
	{ 0x8411c42c, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0xe39944e, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0xfffe441d, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0xd1bc3138, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0xd8e484f0, __VMLINUX_SYMBOL_STR(register_chrdev_region) },
	{ 0xd2dccc43, __VMLINUX_SYMBOL_STR(init_timer_key) },
	{ 0xd6b8e852, __VMLINUX_SYMBOL_STR(request_threaded_irq) },
	{ 0xbbe23ac6, __VMLINUX_SYMBOL_STR(irq_of_parse_and_map) },
	{ 0x62f243bc, __VMLINUX_SYMBOL_STR(gpiod_direction_input) },
	{ 0x47229b5c, __VMLINUX_SYMBOL_STR(gpio_request) },
	{ 0x91715312, __VMLINUX_SYMBOL_STR(sprintf) },
	{ 0xfa2a45e, __VMLINUX_SYMBOL_STR(__memzero) },
	{ 0x901d190b, __VMLINUX_SYMBOL_STR(of_get_named_gpio_flags) },
	{ 0x37d0e3f1, __VMLINUX_SYMBOL_STR(of_find_node_opts_by_path) },
	{ 0xb7f1b096, __VMLINUX_SYMBOL_STR(kill_fasync) },
	{ 0x423af172, __VMLINUX_SYMBOL_STR(gpiod_get_raw_value) },
	{ 0xf5fb58c0, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0xda02d67, __VMLINUX_SYMBOL_STR(jiffies) },
	{ 0xa18e1a83, __VMLINUX_SYMBOL_STR(mod_timer) },
	{ 0x6ec355df, __VMLINUX_SYMBOL_STR(fasync_helper) },
	{ 0xaad8c7d6, __VMLINUX_SYMBOL_STR(default_wake_function) },
	{ 0x6f2c75e3, __VMLINUX_SYMBOL_STR(remove_wait_queue) },
	{ 0x5f7b007f, __VMLINUX_SYMBOL_STR(add_wait_queue) },
	{ 0xe0de58d6, __VMLINUX_SYMBOL_STR(finish_wait) },
	{ 0x6d3d18f5, __VMLINUX_SYMBOL_STR(prepare_to_wait_event) },
	{ 0x1000e51, __VMLINUX_SYMBOL_STR(schedule) },
	{ 0xfe487975, __VMLINUX_SYMBOL_STR(init_wait_entry) },
	{ 0xf4fa543b, __VMLINUX_SYMBOL_STR(arm_copy_to_user) },
	{ 0xb70789e, __VMLINUX_SYMBOL_STR(__might_fault) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xa1c76e0a, __VMLINUX_SYMBOL_STR(_cond_resched) },
	{ 0xefd6cf06, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr0) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

