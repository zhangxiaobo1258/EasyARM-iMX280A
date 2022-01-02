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
	{ 0x9a8bbb3e, __VMLINUX_SYMBOL_STR(input_free_device) },
	{ 0x6ada72f2, __VMLINUX_SYMBOL_STR(input_unregister_device) },
	{ 0xfe990052, __VMLINUX_SYMBOL_STR(gpio_free) },
	{ 0xc1514a3b, __VMLINUX_SYMBOL_STR(free_irq) },
	{ 0xd211501, __VMLINUX_SYMBOL_STR(del_timer) },
	{ 0xdb0fd1b6, __VMLINUX_SYMBOL_STR(input_register_device) },
	{ 0x6d814af, __VMLINUX_SYMBOL_STR(input_set_capability) },
	{ 0x8cc7e2c5, __VMLINUX_SYMBOL_STR(input_allocate_device) },
	{ 0xd2dccc43, __VMLINUX_SYMBOL_STR(init_timer_key) },
	{ 0xd6b8e852, __VMLINUX_SYMBOL_STR(request_threaded_irq) },
	{ 0xbbe23ac6, __VMLINUX_SYMBOL_STR(irq_of_parse_and_map) },
	{ 0x62f243bc, __VMLINUX_SYMBOL_STR(gpiod_direction_input) },
	{ 0x47229b5c, __VMLINUX_SYMBOL_STR(gpio_request) },
	{ 0x91715312, __VMLINUX_SYMBOL_STR(sprintf) },
	{ 0xfa2a45e, __VMLINUX_SYMBOL_STR(__memzero) },
	{ 0x901d190b, __VMLINUX_SYMBOL_STR(of_get_named_gpio_flags) },
	{ 0x37d0e3f1, __VMLINUX_SYMBOL_STR(of_find_node_opts_by_path) },
	{ 0xda02d67, __VMLINUX_SYMBOL_STR(jiffies) },
	{ 0xa18e1a83, __VMLINUX_SYMBOL_STR(mod_timer) },
	{ 0xefd6cf06, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr0) },
	{ 0x350ab6e8, __VMLINUX_SYMBOL_STR(input_event) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x423af172, __VMLINUX_SYMBOL_STR(gpiod_get_raw_value) },
	{ 0xf5fb58c0, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

