CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
FLASH   = st-flash

MCUFLAGS = -mcpu=cortex-m4 -mthumb
CFLAGS   = $(MCUFLAGS) \
           -DSTM32F446xx \
           -ICMSIS/Core/Include \
           -ICMSIS/Device/Include \
           -Wall \
           -O0 \
           -g3 \
           -ffunction-sections \
           -fdata-sections

LDFLAGS = $(MCUFLAGS) \
          -TSTM32F446RETX_FLASH.ld \
          -Wl,--gc-sections \
          -Wl,-Map=build/firmware.map \
          --specs=nosys.specs

BUILD = build

OBJS = $(BUILD)/main.o \
       $(BUILD)/system_stm32f4xx.o \
       $(BUILD)/startup_stm32f446xx.o

all: $(BUILD)/firmware.bin

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/main.o: src/main.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/system_stm32f4xx.o: system/system_stm32f4xx.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/startup_stm32f446xx.o: startup/startup_stm32f446xx.s | $(BUILD)
	$(CC) $(MCUFLAGS) -c $< -o $@

$(BUILD)/firmware.elf: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(OBJCOPY) -O binary $< $@

flash: $(BUILD)/firmware.bin
	$(FLASH) --reset write $< 0x08000000

clean:
	rm -rf $(BUILD)

.PHONY: all flash clean
