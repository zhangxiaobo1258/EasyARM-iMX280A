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
	{ 0x7a712717, __VMLINUX_SYMBOL_STR(snd_soc_put_enum_double) },
	{ 0xf70e2e01, __VMLINUX_SYMBOL_STR(snd_soc_get_enum_double) },
	{ 0xbf48e27, __VMLINUX_SYMBOL_STR(snd_soc_put_volsw) },
	{ 0x360d45f1, __VMLINUX_SYMBOL_STR(snd_soc_get_volsw) },
	{ 0xe166206f, __VMLINUX_SYMBOL_STR(snd_soc_info_volsw) },
	{ 0x13c62b28, __VMLINUX_SYMBOL_STR(snd_soc_dapm_put_enum_double) },
	{ 0xd0cccb1b, __VMLINUX_SYMBOL_STR(snd_soc_dapm_get_enum_double) },
	{ 0x6820659, __VMLINUX_SYMBOL_STR(snd_soc_info_enum_double) },
	{ 0x7c9d499a, __VMLINUX_SYMBOL_STR(i2c_del_driver) },
	{ 0xa75f8509, __VMLINUX_SYMBOL_STR(i2c_register_driver) },
	{ 0x7ec0fff2, __VMLINUX_SYMBOL_STR(lockdep_init_map) },
	{ 0x8e865d3c, __VMLINUX_SYMBOL_STR(arm_delay_ops) },
	{ 0xc4f6b4fa, __VMLINUX_SYMBOL_STR(dev_err) },
	{ 0x3d56e28a, __VMLINUX_SYMBOL_STR(gpiod_set_raw_value) },
	{ 0xf5fb58c0, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0x6de1f701, __VMLINUX_SYMBOL_STR(snd_soc_register_codec) },
	{ 0x206fcff8, __VMLINUX_SYMBOL_STR(devm_gpio_request_one) },
	{ 0xeb71639c, __VMLINUX_SYMBOL_STR(devm_kmalloc) },
	{ 0xd3f57a2, __VMLINUX_SYMBOL_STR(_find_next_bit_le) },
	{ 0x6d662533, __VMLINUX_SYMBOL_STR(_find_first_bit_le) },
	{ 0x676bbc0f, __VMLINUX_SYMBOL_STR(_set_bit) },
	{ 0x49ebacbd, __VMLINUX_SYMBOL_STR(_clear_bit) },
	{ 0xf3d78a8a, __VMLINUX_SYMBOL_STR(i2c_master_recv) },
	{ 0x769e3306, __VMLINUX_SYMBOL_STR(i2c_master_send) },
	{ 0x2d3385d3, __VMLINUX_SYMBOL_STR(system_wq) },
	{ 0xf228f941, __VMLINUX_SYMBOL_STR(queue_work_on) },
	{ 0x17c875d4, __VMLINUX_SYMBOL_STR(snd_soc_unregister_codec) },
	{ 0xefd6cf06, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr0) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("of:N*T*Cnxp,uda1380");
MODULE_ALIAS("of:N*T*Cnxp,uda1380C*");
MODULE_ALIAS("i2c:uda1380");
