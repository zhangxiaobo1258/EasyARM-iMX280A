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
	{ 0x4d768026, __VMLINUX_SYMBOL_STR(platform_driver_unregister) },
	{ 0x31fa9e87, __VMLINUX_SYMBOL_STR(__platform_driver_register) },
	{ 0x8411c42c, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0xe39944e, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0xfffe441d, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0xd1bc3138, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0xd8e484f0, __VMLINUX_SYMBOL_STR(register_chrdev_region) },
	{ 0x79c5a9f0, __VMLINUX_SYMBOL_STR(ioremap) },
	{ 0xc4f6b4fa, __VMLINUX_SYMBOL_STR(dev_err) },
	{ 0x1119e449, __VMLINUX_SYMBOL_STR(platform_get_resource) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xa5a01498, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x42e83abd, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0x269302bc, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0xedc03953, __VMLINUX_SYMBOL_STR(iounmap) },
	{ 0xefd6cf06, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr0) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

