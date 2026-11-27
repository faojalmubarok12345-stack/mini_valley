# Nama ROM hasil build (.nds)
TARGET		:= mini-valley-ds

# Variabel folder resmi BlocksDS
SOURCEDIRS	:= source
GFXDIRS		:= gfx
BINDIRS		:= data
BUILD		:= build

# Toolchain BlocksDS / Wonderful
BLOCKSDS		?= /opt/blocksds/core
WONDERFUL_TOOLCHAIN	?= /opt/wonderful
ARM_NONE_EABI_PATH	?= $(WONDERFUL_TOOLCHAIN)/toolchain/gcc-arm-none-eabi/bin/

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
