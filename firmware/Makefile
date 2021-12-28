TARGET = firmware
DEBUG = 1
OPT = -Og

BUILD_DIR = target

C_SOURCES = \
  old/Core/Src/main.c \
  old/Core/Src/system_stm32g0xx.c \
  libs/stm32/Src/stm32g0xx_ll_utils.c \
  libs/stm32/Src/stm32g0xx_ll_exti.c \
  libs/stm32/Src/stm32g0xx_ll_gpio.c \
  libs/stm32/Src/stm32g0xx_ll_adc.c \
  libs/stm32/Src/stm32g0xx_ll_rcc.c \
  libs/stm32/Src/stm32g0xx_ll_dma.c \
  libs/stm32/Src/stm32g0xx_ll_tim.c \
  libs/stm32/Src/stm32g0xx_ll_usart.c \
  libs/stm32/Src/stm32g0xx_ll_pwr.c \
  src/current_sensor.c \
  src/debug.c \
  src/firmware_it.c \
  src/firmware.c \
  src/ir_rx.c \
  src/ir_tx.c \
  src/rpi.c \
  src/time.c \
  src/uint8_ring_buffer.c \
  src/uint32_ring_buffer.c \
  src/usart_it.c

ASM_SOURCES = \
  src/startup_stm32g031xx.s

PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
CPU = -mcpu=cortex-m0plus
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

AS_DEFS = 

C_DEFS =  \
  -DUSE_FULL_LL_DRIVER \
  -DSTM32G031xx \
  -DHSE_VALUE=8000000 \
  -DHSE_STARTUP_TIMEOUT=100 \
  -DLSE_STARTUP_TIMEOUT=5000 \
  -DLSE_VALUE=32768 \
  -DEXTERNAL_CLOCK_VALUE=12288000 \
  -DHSI_VALUE=16000000 \
  -DLSI_VALUE=32000 \
  -DVDD_VALUE=3300 \
  -DPREFETCH_ENABLE=0 \
  -DINSTRUCTION_CACHE_ENABLE=1 \
  -DDATA_CACHE_ENABLE=1

AS_INCLUDES = 

C_INCLUDES =  \
  -Iold/Core/Inc \
  -Ilibs/stm32/Inc \
  -Iold/Drivers/CMSIS/Device/ST/STM32G0xx/Include \
  -Iold/Drivers/CMSIS/Include

ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

LDSCRIPT = old/STM32G031F4Px_FLASH.ld

LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--print-memory-usage,--gc-sections

all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@		

clean:
	-rm -fR $(BUILD_DIR)
  
-include $(wildcard $(BUILD_DIR)/*.d)
