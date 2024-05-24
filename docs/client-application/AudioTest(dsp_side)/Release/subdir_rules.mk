################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-c6000_8.3.10/bin/cl6x" -mv6600 -O2 --include_path="E:/Projects/workspace_SVT2/AudioTest" --include_path="E:/Projects/workspace_SVT2/AudioTest/DigitalCoreLibs/speex-1.2rc1/libspeex" --include_path="C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-c6000_8.3.10/include" --define=am5718 --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-1467392783:
	@$(MAKE) --no-print-directory -Onone -f subdir_rules.mk build-1467392783-inproc

build-1467392783-inproc: ../AudioTest.cfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: XDCtools'
	"C:/ti/ccs1040/xdctools_3_62_01_15_core/xs" --xdcpath="E:/ti/processor_sdk_rtos_am57xx_08_01_00_09/openmp_dsp_am57xx_2_06_03_00/packages;E:/ti/processor_sdk_rtos_am57xx_08_01_00_09/pdk_am57xx_1_0_17/packages;E:/ti/processor_sdk_rtos_am57xx_08_01_00_09/ctoolslib_2_2_0_0/packages;E:/ti/processor_sdk_rtos_am57xx_08_01_00_09/edma3_lld_2_12_05_30E/packages;/packages;/examples;/packages;E:/ti/processor_sdk_rtos_am57xx_08_01_00_09/imglib_c66x_3_1_1_0/packages;E:/ti/processor_sdk_rtos_am57xx_08_01_00_09/ipc_3_50_04_08/packages;E:/ti/processor_sdk_rtos_am57xx_08_01_00_09/ndk_3_61_01_01/packages;E:/ti/processor_sdk_rtos_am57xx_08_01_00_09/bios_6_76_03_01/packages;" xdc.tools.configuro -o configPkg -t ti.targets.elf.C66 -p g -r release -b "E:/Projects/workspace_SVT2/AudioTest/config.bld" -c "C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-c6000_8.3.10" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

configPkg/linker.cmd: E:/Projects/workspace_SVT2/AudioTest/config.bld
configPkg/linker.cmd: build-1467392783 ../AudioTest.cfg E:/Projects/workspace_SVT2/AudioTest/config.bld
configPkg/compiler.opt: build-1467392783 E:/Projects/workspace_SVT2/AudioTest/config.bld
configPkg/: build-1467392783 E:/Projects/workspace_SVT2/AudioTest/config.bld

I2C_soc.obj: E:/ti/processor_sdk_rtos_am57xx_08_01_00_09/pdk_am57xx_1_0_17/packages/ti/drv/i2c/soc/am572x/I2C_soc.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-c6000_8.3.10/bin/cl6x" -mv6600 -O2 --include_path="E:/Projects/workspace_SVT2/AudioTest" --include_path="E:/Projects/workspace_SVT2/AudioTest/DigitalCoreLibs/speex-1.2rc1/libspeex" --include_path="C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-c6000_8.3.10/include" --define=am5718 --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


