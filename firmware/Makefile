TARGET = firmware
DEBUG = 1
OPT = -Og

BUILD_DIR = target

C_SOURCES = \
  src/stm32/system_stm32g0xx.c \
  libs/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_utils.c \
  libs/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_exti.c \
  libs/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_gpio.c \
  libs/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_adc.c \
  libs/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_rcc.c \
  libs/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_dma.c \
  libs/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_tim.c \
  libs/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_usart.c \
  libs/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_pwr.c

CPP_SOURCES = \
  src/main.cpp \
  src/setup.cpp \
  src/ir_rx.cpp \
  src/main_it.cpp \
  src/ir_tx.cpp \
  src/uint8_ring_buffer.cpp \
  src/uint32_ring_buffer.cpp \
  src/current_sensor.cpp \
  src/hal/Bus.cpp \
  src/hal/NVIC.cpp \
  src/hal/RCC.cpp \
  src/hal/Clocks.cpp \
  src/hal/USART.cpp \
  src/hal/System.cpp \
  src/hal/GPIO.cpp \
  src/hal/Timer.cpp \
  src/hal/DMA.cpp \
  src/hal/IWDG.cpp

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
  -Isrc \
  -Ilibs/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Inc \
  -Ilibs/STM32CubeG0/Drivers/CMSIS/Device/ST/STM32G0xx/Include \
  -Ilibs/STM32CubeG0/Drivers/CMSIS/Include \
  -Ilibs/etl/include

ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -fno-exceptions
CPPFLAGS += -fno-rtti

LDSCRIPT = src/STM32G031F4Px_FLASH.ld

LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--print-memory-usage,--gc-sections

all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

MKDIR_P ?= mkdir -p

OBJECTS = $(addprefix $(BUILD_DIR)/,$(C_SOURCES:.c=.o))
vpath %.c $(sort $(dir $(C_SOURCES)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(CPP_SOURCES:.cpp=.o))
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(ASM_SOURCES:.s=.o))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(MKDIR_P) $(dir $@)
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(<:.c=.lst) $< -o $@

$(BUILD_DIR)/%.o: %.cpp Makefile | $(BUILD_DIR) 
	$(MKDIR_P) $(dir $@)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(<:.cpp=.lst) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(MKDIR_P) $(dir $@)
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
