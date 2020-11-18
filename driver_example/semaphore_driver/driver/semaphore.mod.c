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
	{ 0x8411c42c, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0xe39944e, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0xfffe441d, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0xd1bc3138, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0xd8e484f0, __VMLINUX_SYMBOL_STR(register_chrdev_region) },
	{ 0x5d56bb26, __VMLINUX_SYMBOL_STR(gpiod_direction_output_raw) },
	{ 0x901d190b, __VMLINUX_SYMBOL_STR(of_get_named_gpio_flags) },
	{ 0x37d0e3f1, __VMLINUX_SYMBOL_STR(of_find_node_opts_by_path) },
	{ 0x7ec0fff2, __VMLINUX_SYMBOL_STR(lockdep_init_map) },
	{ 0x3d56e28a, __VMLINUX_SYMBOL_STR(gpiod_set_raw_value) },
	{ 0xf5fb58c0, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xa6405bf5, __VMLINUX_SYMBOL_STR(down_interruptible) },
	{ 0x1dde3e95, __VMLINUX_SYMBOL_STR(up) },
	{ 0xefd6cf06, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr0) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

