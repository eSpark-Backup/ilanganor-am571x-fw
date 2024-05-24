#
_XDCBUILDCOUNT = 0
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = E:/ti/processor_sdk_rtos_am57xx_08_01_00_09/openmp_dsp_am57xx_2_06_03_00/packages;E:/ti/processor_sdk_rtos_am57xx_08_01_00_09/pdk_am57xx_1_0_17/packages;E:/ti/processor_sdk_rtos_am57xx_08_01_00_09/ctoolslib_2_2_0_0/packages;E:/ti/processor_sdk_rtos_am57xx_08_01_00_09/edma3_lld_2_12_05_30E/packages;E:/packages;E:/examples;E:/ti/processor_sdk_rtos_am57xx_08_01_00_09/imglib_c66x_3_1_1_0/packages;E:/ti/processor_sdk_rtos_am57xx_08_01_00_09/ipc_3_50_04_08/packages;E:/ti/processor_sdk_rtos_am57xx_08_01_00_09/ndk_3_61_01_01/packages;E:/ti/processor_sdk_rtos_am57xx_08_01_00_09/bios_6_76_03_01/packages;C:/Users/gili_k/myRepository/packages;C:/ti/volib_C66_2_1_0_1/packages;E:/Projects/workspace_SVT2/AudioTest/.config
override XDCROOT = C:/ti/ccs1040/xdctools_3_62_01_15_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = E:/ti/processor_sdk_rtos_am57xx_08_01_00_09/openmp_dsp_am57xx_2_06_03_00/packages;E:/ti/processor_sdk_rtos_am57xx_08_01_00_09/pdk_am57xx_1_0_17/packages;E:/ti/processor_sdk_rtos_am57xx_08_01_00_09/ctoolslib_2_2_0_0/packages;E:/ti/processor_sdk_rtos_am57xx_08_01_00_09/edma3_lld_2_12_05_30E/packages;E:/packages;E:/examples;E:/ti/processor_sdk_rtos_am57xx_08_01_00_09/imglib_c66x_3_1_1_0/packages;E:/ti/processor_sdk_rtos_am57xx_08_01_00_09/ipc_3_50_04_08/packages;E:/ti/processor_sdk_rtos_am57xx_08_01_00_09/ndk_3_61_01_01/packages;E:/ti/processor_sdk_rtos_am57xx_08_01_00_09/bios_6_76_03_01/packages;C:/Users/gili_k/myRepository/packages;C:/ti/volib_C66_2_1_0_1/packages;E:/Projects/workspace_SVT2/AudioTest/.config;C:/ti/ccs1040/xdctools_3_62_01_15_core/packages;..
HOSTOS = Windows
endif
