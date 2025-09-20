
# ASM source files
ASRC += ${CH32LIBSDK_DEVICES_DIR}/babypc/babypc_vga_asm.S

# C source files
ifeq (${OS},Windows_NT)
CSRC += ${CH32LIBSDK_DEVICES_DIR}/babypc/babypc.c
else
CSRC += ${CH32LIBSDK_DEVICES_DIR}/babypc/babypc_vga.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/babypc/babypc_draw.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/babypc/babypc_key.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/babypc/babypc_snd.c
CSRC += ${CH32LIBSDK_DEVICES_DIR}/babypc/babypc_init.c
endif

DEFINE += -D USE_BABYPC=1

MCU=CH32V002x4

# reduce program size
EXTRA_AFLAGS += -D REDUCE_CRT0_SIZE=1
