################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-c6000_8.3.10

GEN_OPTS__FLAG := --cmd_file="configPkg/compiler.opt" 
GEN_CMDS__FLAG := -l"configPkg/linker.cmd" 

ORDERED_OBJS += \
"./AudioCfg.obj" \
"./AudioInProcess.obj" \
"./I2C_soc.obj" \
"./InBuff.obj" \
"./__veusim.obj" \
"./aic12.obj" \
"./audioSample_io.obj" \
"./audio_evmInit.obj" \
"./audio_mux.obj" \
"./audio_processing.obj" \
"./caller_id.obj" \
"./codec_if.obj" \
"./delay_buffer.obj" \
"./dtmf_det.obj" \
"./evmAM571x_pinmux.obj" \
"./filter.obj" \
"./filter_configured.obj" \
"./filter_dc.obj" \
"./filter_navy_headset.obj" \
"./g711.obj" \
"./host_ctrl.obj" \
"./host_frame.obj" \
"./lowcfe.obj" \
"./main.obj" \
"./maleTest16.obj" \
"./mcasp_cfg.obj" \
"./mcasp_soc.obj" \
"./plc_fp.obj" \
"./profiling.obj" \
"./rsc_table_vayu_dsp.obj" \
"./rsc_types.obj" \
"./system_config.obj" \
"./tone_generate.obj" \
"./veuAlg.obj" \
"./veusim.obj" \
$(GEN_CMDS__FLAG) \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
AudioTest.out \

EXE_OUTPUTS__QUOTED += \
"AudioTest.out" \

BIN_OUTPUTS += \
AudioTest.hex \

BIN_OUTPUTS__QUOTED += \
"AudioTest.hex" \


# All Target
all: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "AudioTest.out"

# Tool invocations
AudioTest.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: C6000 Linker'
	"C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-c6000_8.3.10/bin/cl6x" -mv6600 -O2 --define=am5718 --diag_warning=225 --diag_wrap=off --display_error_number -z -m"AudioTest.map" --heap_size=0x800 --stack_size=0x800 -i"C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-c6000_8.3.10/lib" -i"C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-c6000_8.3.10/include" --reread_libs --diag_wrap=off --display_error_number --warn_sections --xml_link_info="AudioTest_linkInfo.xml" --rom_model -o "AudioTest.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

AudioTest.hex: $(EXE_OUTPUTS)
	@echo 'Building secondary target: "$@"'
	@echo 'Invoking: C6000 Hex Utility'
	"C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-c6000_8.3.10/bin/hex6x" --diag_wrap=off -o "AudioTest.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building secondary target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(GEN_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "AudioCfg.obj" "AudioInProcess.obj" "I2C_soc.obj" "InBuff.obj" "__veusim.obj" "aic12.obj" "audioSample_io.obj" "audio_evmInit.obj" "audio_mux.obj" "audio_processing.obj" "caller_id.obj" "codec_if.obj" "delay_buffer.obj" "dtmf_det.obj" "evmAM571x_pinmux.obj" "filter.obj" "filter_configured.obj" "filter_dc.obj" "filter_navy_headset.obj" "g711.obj" "host_ctrl.obj" "host_frame.obj" "lowcfe.obj" "main.obj" "maleTest16.obj" "mcasp_cfg.obj" "mcasp_soc.obj" "plc_fp.obj" "profiling.obj" "rsc_table_vayu_dsp.obj" "rsc_types.obj" "system_config.obj" "tone_generate.obj" "veuAlg.obj" "veusim.obj" 
	-$(RM) "AudioCfg.d" "AudioInProcess.d" "I2C_soc.d" "InBuff.d" "__veusim.d" "aic12.d" "audioSample_io.d" "audio_evmInit.d" "audio_mux.d" "audio_processing.d" "caller_id.d" "codec_if.d" "delay_buffer.d" "dtmf_det.d" "evmAM571x_pinmux.d" "filter.d" "filter_configured.d" "filter_dc.d" "filter_navy_headset.d" "g711.d" "host_ctrl.d" "host_frame.d" "lowcfe.d" "main.d" "maleTest16.d" "mcasp_cfg.d" "mcasp_soc.d" "plc_fp.d" "profiling.d" "rsc_table_vayu_dsp.d" "rsc_types.d" "system_config.d" "tone_generate.d" "veuAlg.d" "veusim.d" 
	-$(RMDIR) $(GEN_MISC_DIRS__QUOTED)
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

