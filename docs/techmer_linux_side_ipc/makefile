# General Project Configuration
CC = /home/administrator/ti-processor-sdk-linux-am57xx-evm-08_02_01_00b/linux-devkit/sysroots/x86_64-arago-linux/usr/bin/arm-none-linux-gnueabihf-gcc
LD = $(CC)
AR = /home/administrator/ti-processor-sdk-linux-am57xx-evm-08_02_01_00b/linux-devkit/sysroots/x86_64-arago-linux/usr/bin/arm-none-linux-gnueabihf-ar
RM = rm -f
RMDIR = rm -rf
MKDIR = mkdir -p
CP = cp
ECHO = echo

# Path to the development kit and the sysroots
DEVKIT_PATH = /home/administrator/processor_sdk_rtos_am57xx_08_01_00_09/ipc_3_50_04_08/linux
SYSROOT_PATH = /home/administrator/ti-processor-sdk-linux-am57xx-evm-08_02_01_00b/linux-devkit/sysroots/armv7at2hf-neon-linux-gnueabi

# Compiler and Linker Configurations
CPPFLAGS = -D_REENTRANT -I$(DEVKIT_PATH)/include -I$(SYSROOT_PATH)/usr/include
CFLAGS = --sysroot=$(SYSROOT_PATH) $(CCPROFILE_$(PROFILE))
LDFLAGS = -L$(DEVKIT_PATH)/src/api/.libs -L$(DEVKIT_PATH)/src/utils/.libs -L$(DEVKIT_PATH)/src/transport/.libs -L$(SYSROOT_PATH)/usr/lib --sysroot=$(SYSROOT_PATH)

# Libraries to link against
LDLIBS = -lc -lpthread -lrt -ltiipc -ltiipcutils -ltitransportrpmsg

# Conditional Debug and Release Settings
CCPROFILE_debug = -ggdb -DDEBUG
CCPROFILE_release = -O3 -DNDEBUG
LDPROFILE_debug = -ggdb
LDPROFILE_release = -O3

# Object Files and Executables Directories
OBJ_DIR = bin/$(PROFILE)/obj
BIN_DIR = bin/$(PROFILE)

# Source Files
SRCS = main_host.c App.c
OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.ov7A)

# Default target
all: debug release

debug release:
	@$(MAKE) app_host PROFILE=$@

app_host: $(BIN_DIR)/app_host

$(BIN_DIR)/app_host: $(OBJS)
	@$(ECHO) "Linking $@..."
	$(LD) $(LDFLAGS) $(OBJS) $(LDPROFILE_$(PROFILE)) -o $@ $(LDLIBS)

$(OBJ_DIR)/%.ov7A: %.c
	@$(ECHO) "Compiling $<..."
	@$(MKDIR) $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	@$(ECHO) "Cleaning..."
	$(RMDIR) bin

install: $(BIN_DIR)/app_host
	@$(ECHO) "Installing..."
	@$(MKDIR) $(DESTDIR)/$(PROFILE)
	$(CP) $< $(DESTDIR)/$(PROFILE)/

# Dependencies
-include $(OBJS:.ov7A=.d)

